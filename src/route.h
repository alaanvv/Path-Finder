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

static void dfs(int u, int end, Relation* r, int* visited, int* path, int path_len, RouteList* routes) {
  // Adiciona vértice atual no caminho
  path[path_len++] = u;
  if (u == end) {
    // Se tiver alcançado o final, copia o caminho pra lista de rotas
    routes->paths[routes->count] = (int*) malloc(sizeof(int) * (path_len));
    for (int i = 0; i < path_len; i++)
      routes->paths[routes->count][i] = path[i];

    routes->sizes[routes->count++] = path_len;
  } else {
    // Senão, marca o vértice como visitado e passa pro próximo vértice adjacente
    visited[u] = 1;
    for (int v = 0; v < r->size; v++)
      if (relation_has(r, u, v) &&  !visited[v])
        dfs(v, end, r, visited, path, path_len, routes);
  }

  // Ao retornar da função, desmarca o vértice como visitado
  visited[u] = 0;
}

// Função que recebe a matriz de adjacência e retorna uma lista de rotas
RouteList find_all_routes(int start, int end, int n, Relation* r) {
  RouteList routes;
  routes.paths = (int**) malloc(sizeof(int*) * 2048);
  routes.sizes = (int*)  malloc(sizeof(int)  * 2048);
  routes.count = 0;

  int* visited = (int*) calloc(n,  sizeof(int));
  int* path =    (int*) malloc(n * sizeof(int));

  dfs(start, end, r, visited, path, 0, &routes);

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
