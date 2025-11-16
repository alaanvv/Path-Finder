#ifndef RELATION_H
#define RELATION_H

#include <stdlib.h>

typedef struct {
  int **mat, size;
} Relation;

// Relações definidas por matriz de adjacência

Relation relation_create(int size) {
  Relation r;
  r.size = size;
  r.mat = (int**) malloc(size * sizeof(int*));
  for (int i = 0; i < size; i++)
    r.mat[i] = (int*) calloc(size, sizeof(int));
  return r;
}

void relation_add(Relation* r, int u, int v) {
  r->mat[u][v] = 1;
  r->mat[v][u] = 1;
}

void relation_rem(Relation* r, int u, int v) {
  r->mat[u][v] = 0;
  r->mat[v][u] = 0;
}

int relation_has(Relation* r, int u, int v) {
  return r->mat[u][v];
}

void relation_toggle(Relation* r, int u, int v) {
  if (relation_has(r, u, v)) relation_rem(r, u, v);
  else                       relation_add(r, u, v);
}

void relation_free(Relation* r) {
  for (int i = 0; i < r->size; i++) free(r->mat[i]);
  free(r->mat);
}

#endif
