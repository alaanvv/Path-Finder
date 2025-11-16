#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "combinatorial.h"
#include "cartesian.h"
#include "relation.h"
#include "matrix.h"
#include "render.h"
#include "route.h"
#include "set.h"

#define MAGENTA "\033[35m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define RED     "\033[31m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"
#define CLEAR   "\033[H\033[J"

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)
#define RAND(min, max) (rand() % (max - min) + min)
#define PRINT(...) { printf(__VA_ARGS__); printf("\n"); }
#define PRINTLN(...) { printf(__VA_ARGS__); printf("\n"); }
#define ASSERT(x, ...) if (!(x)) { PRINT(__VA_ARGS__); exit(1); }
#define ALERT(str) { printf(BOLD RED "  " str "\n" RESET); sleep(1); }
#define STR_REPEAT(pre, str, suf, x) { printf(pre); for (int c = 0; c < x; c++) printf(str); printf(suf); }

typedef char      c8;
typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t    i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef float    f32;
typedef double   f64;

#define X_SCALE    10
#define Y_SCALE     5

typedef enum { SETUP, GRAPH, PATH } DisplayMode;

// ---

Set*     points_of_interest;
Point*   points, *scaled_points;
MatrixF  distances;
Relation relation;
int      n, max_x = 0, max_y = 0;

// ---

void draw_display(DisplayMode mode, RouteList l, int route_i) {
  printf(CLEAR);

  // Inicializa o display com caracteres vazios
  MatrixS display = matrixs_init((max_y + 1) * Y_SCALE, (max_x + 1) * X_SCALE);
  for (int i = 0; i < display.rows; i++)
    for (int j = 0; j < display.cols; j++)
      matrixs_set(&display, i, j, " ");

  // Se estiver no modo grafo, desenha todos caminhos
  if (mode == GRAPH)
    for (u8 u = 0; u < n; u++)
      for (u8 v = u + 1; v < n; v++)
        if (relation_has(&relation, u, v))
          draw_line(&display, scaled_points[u], scaled_points[v]);

  // Se estiver no modo caminho, desenha o caminho atual
  if (mode == PATH)
    for (u8 i = 0; i + 1 < l.sizes[route_i]; i++)
      draw_line(&display, scaled_points[l.paths[route_i][i]], scaled_points[l.paths[route_i][i + 1]]);

  // Se estiver no modo setup, desenha todas cidades
  if (mode == SETUP)
    for (int i = 0; i < n; i++)
      draw_vertex(&display, scaled_points[i], i, set_has(points_of_interest, i) ? MAGENTA : "", set_has(points_of_interest, i));

  // Se estiver no modo grafo, desenha apenas pontos de interesse
  if (mode == GRAPH)
    for (int i = 0; i < n; i++)
      if (set_has(points_of_interest, i))
        draw_vertex(&display, scaled_points[i], i, MAGENTA, 0);

  // Se estiver no modo caminho, desenha apenas as cidades no caminho atual
  if (mode == PATH)
    for (int _i = 0; _i < l.sizes[route_i]; _i++) {
      u8 i = l.paths[route_i][_i];
      draw_vertex(&display, scaled_points[i], i, _i == 0 ? GREEN : _i == l.sizes[route_i] - 1 ? BLUE : MAGENTA, 0);
    }

  // Imprime o display no terminal
  STR_REPEAT("\n  ", "▓", "\n", display.cols + 4);
  for (int i = 0; i < display.rows; i++) {
    printf("  ▓▓");
    for (int j = 0; j < display.cols; j++) printf("%s", matrixs_get(&display, i, j));
    printf("▓▓\n");
  }
  STR_REPEAT("  ", "▓", "\n\n", display.cols + 4);
  matrixs_free(&display);
}

void draw_routes(RouteList l) {
  if (l.count == 0) {
    ALERT("Nenhuma rota encontrada");
    return;
  }

  // Calcula qual caminho tem a menor distância
  float best_distance = 1e18;
  int best_i = -1;
  for (int i = 0; i < l.count; i++) {
    float t = 0;
    for (int j = 0; j + 1 < l.sizes[i]; j++)
      t += matrixf_get(&distances, l.paths[i][j], l.paths[i][j + 1]);
    if (t > best_distance) continue;
    best_distance = t;
    best_i = i;
  }

  for (int route_i = 0; route_i < l.count; route_i++) {
    // Desenha o caminho
    draw_display(PATH, l, route_i);

    float total = 0;

    // Imprime o caminho nesse formato "01 -> 12 -> 08" e calcula distância
    printf("\n  ");
    for (int j = 0; j < l.sizes[route_i]; j++) {
      int u = l.paths[route_i][j];
      printf(BOLD "%s%d " RESET, j == 0 ? GREEN : (j == l.sizes[route_i]-1) ? BLUE : MAGENTA, u);

      if (j + 1 < l.sizes[route_i]) {
        printf("-> ");
        total += matrixf_get(&distances, u, l.paths[route_i][j + 1]);
      }
    }

    printf(BOLD "\n\n  Distância:" RESET " %.2f", total);
    // Se a rota for a mais curta mostra uma mensagem
    if (route_i == best_i) printf(BOLD GREEN "  (MENOR ROTA)" RESET);

    // Aguarda input do usuário pra passar pra próxima rota
    printf("\n\n  << " BOLD "ENTER (%d/%d)" RESET, route_i+1, l.count);
    if (!route_i) scanf("%*c%*c");
    else scanf("%*c");
  }
}

// ---

void setup_menu() {
  draw_display(SETUP, (RouteList) {}, 0);

  PRINT(BOLD "  <id>                      " RESET "Marca/desmarca a cidade <id> como ponto de interesse");
  PRINT(BOLD "  run                       " RESET "Inicia o programa");
  printf("\n  :");

  c8 opt[16];
  scanf("%s", opt);

  // Se o comando for run, sai do menu recursivo
  if (!strcmp(opt, "run")) return;
  // Senão converte a entrada pra um número e marca/desmarca o ponto de interesse
  set_toggle(points_of_interest, atoi(opt));

  setup_menu();
}

void main_menu() {
  draw_display(GRAPH, (RouteList){}, 0);

  PRINT(BOLD "  dist                      " RESET "Calcula matriz de distâncias");
  PRINT(BOLD "  conn   <id1> <id2>        " RESET "Conecta/desconecta dois pontos");
  PRINT(BOLD "  routes <id1> <id2>        " RESET "Calcula todas rotas entre duas cidades");
  PRINT(BOLD "  count  <n> <r>            " RESET "Calcula combinações e permutações");
  PRINT(BOLD "  exit                      " RESET "Encerra o programa");
  printf("\n  :");

  u16 p1, p2;
  c8 opt[16];
  scanf("%s", opt);
 
  if (!strcmp(opt, "dist")) {
    draw_distance_matrix(&distances, points_of_interest, X_SCALE);
  }  
  else if (!strcmp(opt, "conn")) {
    scanf("%hu %hu", &p1, &p2);
    if (set_has(points_of_interest, p1) && set_has(points_of_interest, p2))
      relation_toggle(&relation, p1, p2);
    else ALERT("Ponto de interesse não encontrado");
  }
  else if (!strcmp(opt, "routes")) {
    scanf("%hu %hu", &p1, &p2);
    if (set_has(points_of_interest, p1) && set_has(points_of_interest, p2)) {
      // Chama a função que calcula as rotas e desenha elas
      RouteList l = find_all_routes(p1, p2, p1, &relation);
      draw_routes(l);
      routelist_free(&l);
    }
    else ALERT("Ponto de interesse não encontrado");
  }
  else if (!strcmp(opt, "count")) {
    scanf("%hu %hu", &p1, &p2);
    draw_combinatorial(p1, p2, combinations(p1, p2), permutations(p1, p2));
  }
  else if (!strcmp(opt, "exit")) return;

  main_menu();
}

// ---

int main() {
  FILE *f = fopen("instancia.txt", "r");
  ASSERT(f, "Arquivo instancia.txt não encontrado");

  ASSERT(fscanf(f, "%d", &n) == 1, "Arquivo inválido");
  points = malloc(sizeof(Point) * n);
  scaled_points = malloc(sizeof(Point) * n);

  for (int i = 0; i < n; i++) {
    ASSERT(fscanf(f, "%d %d", &points[i].x, &points[i].y) == 2, "Arquivo inválido");
    scaled_points[i].x = points[i].x * X_SCALE + MAX(0, X_SCALE / 2 - 1);
    scaled_points[i].y = points[i].y * Y_SCALE + Y_SCALE / 2;
    max_x = MAX(max_x, points[i].x);
    max_y = MAX(max_y, points[i].y);
  }

  fclose(f);

  // ---

  distances = matrixf_init(n, n);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      matrixf_set(&distances, i, j, distance(points[i], points[j]));

  relation = relation_create(n);

  points_of_interest = set_init(n);

  // ---

  setup_menu();
  main_menu();

  // ---

  free(points);
  matrixf_free(&distances);
  relation_free(&relation);
  set_free(points_of_interest);

  return 0;
}
