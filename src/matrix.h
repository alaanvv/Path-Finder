#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#include <string.h>

// Matrix Float

typedef struct {
  int rows, cols;
  float* data;
} MatrixF;

static inline MatrixF matrixf_init(int r, int c) {
  MatrixF m;
  m.rows = r;
  m.cols = c;
  m.data = (float*) malloc(sizeof(float) * r * c);
  return m;
}

static inline void matrixf_free(MatrixF* m) {
  free(m->data);
  m->data = NULL;
}

static inline void matrixf_set(MatrixF* m, int r, int c, float v) {
  m->data[r * m->cols + c] = v;
}

static inline float matrixf_get(MatrixF* m, int r, int c) {
  return m->data[r * m->cols + c];
}

// Matrix String

typedef struct {
  int rows, cols;
  char (*data)[128];
} MatrixS;

static inline MatrixS matrixs_init(int r, int c) {
  MatrixS m;
  m.rows = r;
  m.cols = c;
  m.data = (char(*)[128]) malloc(sizeof(char[128]) * r * c);
  return m;
}

static inline void matrixs_free(MatrixS* m) {
  free(m->data);
  m->data = NULL;
}

static inline void matrixs_set(MatrixS* m, int r, int c, const char* v) {
  strcpy(m->data[r * m->cols + c], v);
}

static inline char* matrixs_get(MatrixS* m, int r, int c) {
  return m->data[r * m->cols + c];
}

#endif
