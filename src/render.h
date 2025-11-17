#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>

#include "cartesian.h"
#include "matrix.h"
#include "set.h"

#define PRINT(...) { printf(__VA_ARGS__); printf("\n"); }

#ifdef NO_ANSI
  #undef MAGENTA
  #undef GREEN
  #undef BLUE
  #undef RED
  #undef BOLD
  #undef RESET
  #undef CLEAR
  #define MAGENTA ""
  #define GREEN   ""
  #define BLUE    ""
  #define RED     ""
  #define BOLD    ""
  #define RESET   ""
  #define CLEAR   "\n\n\n\n\n\n\n\n"
#else
  #undef MAGENTA
  #undef GREEN
  #undef BLUE
  #undef RED
  #undef BOLD
  #undef RESET
  #undef CLEAR
  #define MAGENTA "\033[35m"
  #define GREEN   "\033[32m"
  #define BLUE    "\033[34m"
  #define RED     "\033[31m"
  #define BOLD    "\033[1m"
  #define RESET   "\033[0m"
  #define CLEAR   "\033[H\033[J"
#endif

// Usa algoritmo de Bresenham pra traçar uma linha na matriz
void draw_line(MatrixS* grid, Point p1, Point p2) {
  int dx =  abs(p2.x - p1.x), sx = p1.x < p2.x ? 1 : -1;
  int dy = -abs(p2.y - p1.y), sy = p1.y < p2.y ? 1 : -1;
  int err = dx + dy;

  while (1) {
    if (p1.x == p2.x && p1.y == p2.y) break;
    if (p1.x % 2 == 0) matrixs_set(grid, p1.y, p1.x, "*");

    int e2 = 2 * err;
    if (e2 >= dy) { err += dy; p1.x += sx; }
    if (e2 <= dx) { err += dx; p1.y += sy; }
  }
}

// Desenha um vértice do grafo, podendo passar a cor em ANSI e se vai ter marcação
void draw_vertex(MatrixS* grid, Point p, int id, char* color, int mark) {
  sprintf(matrixs_get(grid, p.y, p.x), BOLD "%s%c%02d%c" RESET, color, mark ? '>' : ' ', id, mark ? '<' : ' ');
  for (int j = 1; j < 4; j++) matrixs_set(grid, p.y, p.x + j, "");
}

// Desenha uma matriz, marcando vermelho na diagonal principal e filtrando por índice em um conjunto
void draw_matrix(MatrixF* m, Set* filter, int scale) {
  // Desenha os índices no topo
  printf(CLEAR "\n      " BOLD);
  for (int v = 0; v < m->cols; v++)
    if (set_has(filter, v))
      printf(MAGENTA " %02d%*s ", v, scale / 2 - 3, "");
  printf("\n" RESET);

  for (int u = 0; u < m->rows; u++) {
    if (!set_has(filter, u)) continue;
    // Desenha os índices na lateral
    printf(MAGENTA BOLD "\n  %02d  " RESET, u);
    for (int v = 0; v < m->cols; v++) {
      if (!set_has(filter, v)) continue;
      // Deixa a fonte vermelha se estiver na diagonal principal
      if (u == v) printf(RED);
      printf("%*.2f " RESET, scale / 2, matrixf_get(m, u, v));
    }
    printf("\n\n");
  }

  // Aguarda input do usuário pra sair da tela
  printf("  << " BOLD "ENTER " RESET);
  scanf("%*c%*c");
}

// Mostra os resultados da análise combinatória
void draw_combinatorial(int n, int r, int f, int c, int p) {
  // Mostra combinações e permutações
  PRINT(CLEAR);
  PRINT(BOLD "  C(%d, %d)" RESET " = %d", n, r, c);
  PRINT(BOLD "  P(%d, %d)" RESET " = %d", n, r, p);
  PRINT(BOLD "  %d!" RESET " = %d", n,    f);

  // Aguarda input do usuário pra sair da tela
  printf("\n  << " BOLD "ENTER " RESET);
  scanf("%*c%*c");
}

// Desenha as coordenadas dos pontos filtrados pelo conjunto filter
void draw_coords(Point* points, int size, Set* filter) {
  PRINT(CLEAR);
  for (int i = 0; i < size; i++)
    if (set_has(filter, i))
      printf(MAGENTA BOLD "  %02d " RESET "= (%d, %d)\n\n", i, points[i].x, points[i].y);

  // Aguarda input do usuário pra sair da tela
  printf("  << " BOLD "ENTER " RESET);
  scanf("%*c%*c");
}

#endif
