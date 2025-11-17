#ifndef ROUTE_H
#define ROUTE_H

#include <stdlib.h>
#include <stdio.h>

#include "relation.h"

// RouteList é a lista de rotas
typedef struct {
  int **paths; // Um vetor de vetores, cada subvetor é um caminho
  int  *sizes; // Um vetor de tamanhos, cada tamanho corresponde ao vetor de caminho correspondente
  int   count; // Contagem total de caminhos
} RouteList;

static void dfs_debug(int u, int end, Relation* r, int* visited, int* path, int path_len, RouteList* routes) {
  path[path_len++] = u;
  // debug: print current prefix
  fprintf(stderr, "DFS enter: ");
  for (int i=0;i<path_len;i++) fprintf(stderr, "%d ", path[i]);
  fprintf(stderr, "\n");

  if (u == end) {
    routes->paths[routes->count] = malloc(sizeof(int) * path_len);
    for (int i = 0; i < path_len; i++) routes->paths[routes->count][i] = path[i];
    routes->sizes[routes->count++] = path_len;
    fprintf(stderr, "FOUND route %d: ", routes->count-1);
    for (int i=0;i<path_len;i++) fprintf(stderr, "%d ", path[i]);
    fprintf(stderr, "\n");
    return;
  }

  visited[u] = 1;

  for (int v = 0; v < r->size; v++) {
    if (!relation_has(r, u, v)) continue;
    if (visited[v]) { fprintf(stderr, " skip visited %d\n", v); continue; }
    fprintf(stderr, " try edge %d->%d\n", u, v);
    dfs_debug(v, end, r, visited, path, path_len, routes);
  }

  visited[u] = 0;
  fprintf(stderr, "DFS leave: ");
  for (int i=0;i<path_len-1;i++) fprintf(stderr, "%d ", path[i]); // last was u
  fprintf(stderr, "\n");
}


// Função que recebe a matriz de adjacência e retorna uma lista de rotas
RouteList find_all_routes(int start, int end, int n, Relation* r) {
  RouteList routes;
  routes.paths = (int**) malloc(sizeof(int*) * 2048);
  routes.sizes = (int*)  malloc(sizeof(int)  * 2048);
  routes.count = 0;

  int* visited = (int*) calloc(n,  sizeof(int));
  int* path =    (int*) malloc(n * sizeof(int));

  dfs_debug(start, end, r, visited, path, 0, &routes);

  free(visited);
  free(path);
  return routes;
}

void routelist_free(RouteList *l) {
  for (int i = 0; i < l->count; i++) free(l->paths[i]);
  free(l->paths);
  free(l->sizes);
}

#endif
