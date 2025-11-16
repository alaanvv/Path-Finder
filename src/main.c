// TODO Limitar funcoes q recebem ID a usar so pontos de interesse
// TODO refatorar essa grande merda
// TODO comentar tudo
// TODO fazer um readme
// TODO gravar uma demo e screenshots

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#include "combinatorial.h"
#include "cartesian.h"
#include "relation.h"
#include "route.h"
#include "set.h"

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)
#define RAND(min, max) (rand() % (max - min) + min)
#define PRINT(...) { printf(__VA_ARGS__); printf("\n"); }
#define PRINTLN(...) { printf(__VA_ARGS__); printf("\n"); }
#define ASSERT(x, ...) if (!(x)) { PRINT(__VA_ARGS__); exit(1); }

#define MAGENTA "\033[35m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define RED     "\033[31m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

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

#define X_SCALE 10
#define Y_SCALE  5

// ---

Relation r;
double** dist;
Point*   points;
Set      points_of_interest;
int      n;

// ---

void draw_ascii_setup_display() {
  printf("\033[H\033[J");

  // Calcula o tamanho do display
  int W = X_SCALE, H = Y_SCALE;
  for (int i = 0; i < n; i++) {
    W = MAX(W, (points[i].x + 1) * X_SCALE);
    H = MAX(H, (points[i].y + 1) * Y_SCALE);
  }

  // Inicializa o display com caracteres vazios
  char*** grid = malloc(H * sizeof(char**));
  for (int i = 0; i < H; i++) {
    grid[i] = malloc(W * sizeof(char*));
    for (int j = 0; j < W; j++) {
      grid[i][j] = malloc(64 * sizeof(char));
      strcpy(grid[i][j], " ");
    }
  }

  // Coloca C onde tem cidades
  for (int i = 0; i < n; i++) {
    int row = points[i].y * Y_SCALE + Y_SCALE / 2;
    int col = points[i].x * X_SCALE + MAX(0, X_SCALE / 2 - 1);

    strcpy(grid[row][col], BOLD "C");

    char buf[3];
    snprintf(buf, sizeof(buf), "%d", i);
    int k;
    for(k = 0; buf[k] != '\0'; k++)
      grid[row][col + 1 + k][0] = buf[k];
    grid[row][col + 1 + k] = " " RESET;
    if (set_has(&points_of_interest, i)) {
      grid[row][col - 1]     = MAGENTA BOLD ">";
      grid[row][col + 1 + k] = "<" RESET;
    }
  }

  // Desenha o display no terminal
  printf("\n  ");
  for (int j = 0; j < W + 4; j++) printf("▓");
  printf("\n");

  for (int i = 0; i < H; i++) {
    printf("  ▓▓");
    for (int j = 0; j < W; j++) printf("%s", grid[i][j]);
    printf("▓▓\n");
  }

  printf("  ");
  for (int j = 0; j < W + 4; j++) printf("▓");
  printf("\n");

  for (int i = 0; i < H; i++)
    free(grid[i]);
  free(grid);
}

void draw_ascii_display() {
  printf("\033[H\033[J");

  // Calcula o tamanho do display
  int W = X_SCALE, H = Y_SCALE;
  for (int i = 0; i < n; i++) {
    W = MAX(W, (points[i].x + 1) * X_SCALE);
    H = MAX(H, (points[i].y + 1) * Y_SCALE);
  }

  // Inicializa o display com caracteres vazios
  char*** grid = malloc(H * sizeof(char**));
  for (int i = 0; i < H; i++) {
    grid[i] = malloc(W * sizeof(char*));
    for (int j = 0; j < W; j++) {
      grid[i][j] = malloc(64 * sizeof(char));
      strcpy(grid[i][j], " ");
    }
  }

  // Coloca as linhas entre as cidades
  for (u8 u = 0; u < n; u++) {
    for (u8 v = u + 1; v < n; v++) {
      if (!relation_has(&r,u,v)) continue;

      u16 x1 = points[u].x * X_SCALE + MAX(0, X_SCALE / 2 - 1), y1 = points[u].y * Y_SCALE + Y_SCALE / 2;
      u16 x2 = points[v].x * X_SCALE + MAX(0, X_SCALE / 2 - 1), y2 = points[v].y * Y_SCALE + Y_SCALE / 2;

      int x = x1, y = y1;
      int dx =  abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
      int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
      int err = dx + dy;

      while (1) {
        if (x % 2 == 0) strcpy(grid[y][x], "*");
        if (x == x2 && y == y2) break;

        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x += sx; }
        if (e2 <= dx) { err += dx; y += sy; }
      }
    }
  }

  // Coloca C onde tem cidades, S no início e F no fim
  for (int i = 0; i < n; i++) {
    if (!set_has(&points_of_interest, i)) continue;
    int row = points[i].y * Y_SCALE + Y_SCALE / 2;
    int col = points[i].x * X_SCALE + MAX(0, X_SCALE / 2 - 1);

    strcpy(grid[row][col], BOLD "C");

    char buf[3];
    snprintf(buf, sizeof(buf), "%d", i);
    int k;
    for(k = 0; buf[k] != '\0'; k++)
      grid[row][col + 1 + k][0] = buf[k];
    grid[row][col + 1 + k] = " " RESET;
    grid[row][col - 1] = MAGENTA " " BOLD;
    grid[row][col + 1 + k] = " " RESET;
  }

  // Desenha o display no terminal
  printf("\n  ");
  for (int j = 0; j < W + 4; j++) printf("▓");
  printf("\n");

  for (int i = 0; i < H; i++) {
    printf("  ▓▓");
    for (int j = 0; j < W; j++) printf("%s", grid[i][j]);
    printf("▓▓\n");
  }

  printf("  ");
  for (int j = 0; j < W + 4; j++) printf("▓");
  printf("\n");

  for (int i = 0; i < H; i++)
    free(grid[i]);
  free(grid);
}

void draw_ascii_route_display(RouteList l, int idx) {
  printf("\033[H\033[J");

  // Calcula o tamanho do display
  int W = X_SCALE, H = Y_SCALE;
  for (int i = 0; i < n; i++) {
    W = MAX(W, (points[i].x + 1) * X_SCALE);
    H = MAX(H, (points[i].y + 1) * Y_SCALE);
  }

  // Inicializa o display com caracteres vazios
  char*** grid = malloc(H * sizeof(char**));
  for (int i = 0; i < H; i++) {
    grid[i] = malloc(W * sizeof(char*));
    for (int j = 0; j < W; j++) {
      grid[i][j] = malloc(64 * sizeof(char));
      strcpy(grid[i][j], " ");
    }
  }

  // Coloca as linhas entre as cidades
  for (u8 i = 0; i + 1 < l.sizes[idx]; i++) {
    u8 u = l.paths[idx][i];
    u8 v = l.paths[idx][i + 1];

    u16 x1 = points[u].x * X_SCALE + MAX(0, X_SCALE / 2 - 1), y1 = points[u].y * Y_SCALE + Y_SCALE / 2;
    u16 x2 = points[v].x * X_SCALE + MAX(0, X_SCALE / 2 - 1), y2 = points[v].y * Y_SCALE + Y_SCALE / 2;

    int x = x1, y = y1;
    int dx =  abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;

    while (1) {
      if (x % 2 == 0) strcpy(grid[y][x], "*");
      if (x == x2 && y == y2) break;

      int e2 = 2 * err;
      if (e2 >= dy) { err += dy; x += sx; }
      if (e2 <= dx) { err += dx; y += sy; }
    }
  }

  // Coloca C onde tem cidades, S no início e F no fim
  for (u8 _i = 0; _i < l.sizes[idx]; _i++) {
    u8 i = l.paths[idx][_i];
    int row = points[i].y * Y_SCALE + Y_SCALE / 2;
    int col = points[i].x * X_SCALE + MAX(0, X_SCALE / 2 - 1);

    if      (_i == 0) strcpy(grid[row][col], BOLD "S");
    else if (_i == l.sizes[idx] - 1)    strcpy(grid[row][col], BOLD "F");
    else                  strcpy(grid[row][col], BOLD "C");

    char buf[3];
    snprintf(buf, sizeof(buf), "%d", i);
    int k;
    for(k = 0; buf[k] != '\0'; k++)
      grid[row][col + 1 + k][0] = buf[k];
    grid[row][col + 1 + k] = " " RESET;
    if      (i == 0) grid[row][col - 1] = GREEN " " BOLD;
    else if (_i == l.sizes[idx] - 1) grid[row][col - 1] = BLUE " " BOLD;
    else                  grid[row][col - 1] = MAGENTA " " BOLD;
    grid[row][col + 1 + k] = " " RESET;
  }

  // Desenha o display no terminal
  printf("\n  ");
  for (int j = 0; j < W + 4; j++) printf("▓");
  printf("\n");

  for (int i = 0; i < H; i++) {
    printf("  ▓▓");
    for (int j = 0; j < W; j++) printf("%s", grid[i][j]);
    printf("▓▓\n");
  }

  printf("  ");
  for (int j = 0; j < W + 4; j++) printf("▓");
  printf("\n");

  for (int i = 0; i < H; i++)
    free(grid[i]);
  free(grid);
}

void draw_distance_matrix() {
  printf("\033[H\033[J");

  printf("\n      " BOLD);
  for (u8 v = 0; v < n; v++){
    if (!set_has(&points_of_interest, v)) continue;
    int digits = (v == 0) ? 1 : floor(log10(v)) + 1;

    printf(MAGENTA " C%d %*s", v, X_SCALE / 2 -2- digits, "");
  }
  printf("\n" RESET);

  for (u8 u = 0; u < n; u++) {
    if (!set_has(&points_of_interest, u)) continue;
    printf(MAGENTA BOLD "\n  C%d  " RESET, u);
    printf(RESET);
    for (u8 v = 0; v < n; v++) {
      if (!set_has(&points_of_interest, v)) continue;
      if (u == v) printf(RED);
      printf("%*.2f ", X_SCALE / 2, dist[u][v]);
      printf(RESET);
    }
    printf("\n\n");
  }

  printf("  << " BOLD "ENTER " RESET);
  scanf("%*c%*c");
}

void draw_combinatorial(int n, int r) {
  printf("\033[H\033[J");

  int c = combinations(n, r);
  int p = permutations(n, r);

  printf(BOLD"\n  C(%d, %d)"RESET" = %d\n", n, r, c);
  printf(BOLD"  P(%d, %d)"RESET" = %d\n", n, r, p);

  printf("\n  << " BOLD "ENTER" RESET);
  scanf("%*c%*c");
}

void draw_routes(RouteList l) {
  if (l.count == 0) {
    printf(RED BOLD "  Nenhuma rota encontrada!\n" RESET);
    sleep(1);
    return;
  }

  double best = 1e18;
  int best_idx = -1;

  for (int i = 0; i < l.count; i++) {
    double t = 0;
    for (int j = 0; j + 1 < l.sizes[i]; j++) {
      int u = l.paths[i][j];
      int v = l.paths[i][j+1];
      t += dist[u][v];
    }
    if (t < best) {
      best = t;
      best_idx = i;
    }
  }

  for (int idx = 0; idx < l.count; idx++) {
    printf("\033[H\033[J");
    draw_ascii_route_display(l, idx);

    double total = 0;

    printf("\n  ");
    for (int j = 0; j < l.sizes[idx]; j++) {
      int u = l.paths[idx][j];
      printf(BOLD "%s%c%d " RESET,
             j == 0 ? GREEN : (j == l.sizes[idx]-1) ? BLUE : MAGENTA,
             j == 0 ? 'S' : (j == l.sizes[idx]-1) ? 'F' : 'C',
             u);

      if (j+1 < l.sizes[idx]) printf("-> ");
      if (j+1 < l.sizes[idx]) {
        int v = l.paths[idx][j+1];
        total += dist[u][v];
      }
    }

    printf(BOLD "\n\n  Distância:" RESET " %.2f", total);

    if (idx == best_idx)
      printf(BOLD GREEN "  (MENOR ROTA)" RESET);

    printf("\n\n  << " BOLD "ENTER (%d/%d)" RESET, idx+1, l.count);
    if (!idx) scanf("%*c%*c");
    else scanf("%*c");
  }
}


// ---

void draw_setup_menu() {
  draw_ascii_setup_display();

  PRINT("");
  PRINT(BOLD "  <id>                      " RESET "Marca/desmarca a cidade <id> como ponto de interesse");
  PRINT(BOLD "  run                       " RESET "Inicia o programa");
  printf("\n  :");
}

void draw_main_menu() {
  draw_ascii_display();

  PRINT("");
  PRINT(BOLD "  dist                      " RESET "Calcula matriz de distâncias");
  PRINT(BOLD "  conn   <id1> <id2>        " RESET "Conecta/desconecta dois pontos");
  PRINT(BOLD "  routes <id1> <id2>        " RESET "Calcula todas rotas entre duas cidades");
  PRINT(BOLD "  count  <n> <r>            " RESET "Calcula combinações e permutações");
  PRINT(BOLD "  exit                      " RESET "Encerra o programa");
  printf("\n  :");
}

// ---

void setup_menu() {
  char opt[16];
  draw_setup_menu();
  scanf("%s", opt);

  if (!strcmp(opt, "run")) return;
  set_toggle(&points_of_interest, atoi(opt));

  setup_menu();
}

void main_menu() {
  char opt[16];
  u16 a1, a2;
  draw_main_menu();
  scanf("%s", opt);
 
  if (!strcmp(opt, "dist")) {
    draw_distance_matrix();
  }  
  else if (!strcmp(opt, "conn")) {
    scanf("%hu %hu", &a1, &a2);
    relation_toggle(&r, a1, a2);
  }
  else if (!strcmp(opt, "routes")) {
    scanf("%hu %hu", &a1, &a2);
    RouteList l = find_all_routes(a1, a2, n, &r);
    draw_routes(l);

    for (int i = 0; i < l.count; i++)
      free(l.paths[i]);
    free(l.paths);
    free(l.sizes);
  }
  else if (!strcmp(opt, "count")) {
    int nn, rr;
    scanf("%d %d", &nn, &rr);
    draw_combinatorial(nn, rr);
  }
  else if (!strcmp(opt, "exit")) return;

  main_menu();
}

// ---

int main() {
  FILE *f = fopen("instancia.txt", "r");
  ASSERT(f, "Cannot open instancia.txt");

  ASSERT(fscanf(f, "%d", &n) == 1, "Bad file");
  points = malloc(sizeof(Point) * n);

  for (int i = 0; i < n; i++)
    ASSERT(fscanf(f, "%d %d", &points[i].x, &points[i].y) == 2, "Bad file");

  fclose(f);

  // ---

  r = relation_create(n);

  // Cria matriz de distâncias
  dist = malloc(sizeof(double*) * n);
  for (int i = 0; i < n; i++) {
    dist[i] = malloc(sizeof(double) * n);

    for (int j = 0; j < n; j++)
      dist[i][j] = distance(points[i], points[j]);
  }

  // Cria conjunto de pontos de interesse
  points_of_interest.elements = malloc(sizeof(int) * n);
  points_of_interest.size = 0;

  setup_menu();
  main_menu();

  // Cleanup
  for (int i = 0; i < n; i++) {
    free(dist[i]);
  }

  free(dist);
  relation_free(&r);
  free(points);
  free(points_of_interest.elements);

  return 0;
}
