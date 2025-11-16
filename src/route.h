#ifndef ROUTE_H
#define ROUTE_H

#include "relation.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int **paths;
  int *sizes;
  int count;
} RouteList;

static void dfs_rec(int u, int end, Relation *r, int *visited, int *path,
                    int path_len, RouteList *out) {

  if (u == end) {
    out->paths[out->count] = (int *)malloc(sizeof(int) * (path_len + 1));
    for (int i = 0; i < path_len; i++)
      out->paths[out->count][i] = path[i];
    out->paths[out->count][path_len] = u;

    out->sizes[out->count] = path_len + 1;
    out->count++;
    visited[u] = 0;
    return;
  }

  visited[u] = 1;
  path[path_len] = u;

  for (int v = 0; v < r->size; v++) {
    if (!relation_has(r, u, v))
      continue;
    if (visited[v])
      continue;

    dfs_rec(v, end, r, visited, path, path_len + 1, out);
  }

  visited[u] = 0;
}

RouteList find_all_routes(int start, int end, int n, Relation *r) {
  RouteList out;
  out.paths = (int **)malloc(sizeof(int *) * 2048);
  out.sizes = (int *)malloc(sizeof(int) * 2048);
  out.count = 0;

  int *visited = (int *)calloc(n, sizeof(int));
  int *path = (int *)malloc(sizeof(int) * n);

  dfs_rec(start, end, r, visited, path, 0, &out);

  free(visited);
  free(path);
  return out;
}

void routelist_free(RouteList* l) {
  for (int i = 0; i < l->count; i++)
    free(l->paths[i]);
  free(l->paths);
  free(l->sizes);
}

#endif
