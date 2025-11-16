#ifndef SET_H
#define SET_H

#include <stdlib.h>

typedef struct {
  int *elements, size;
} Set;

Set* set_init(int n) {
  Set* set = (Set*) malloc(sizeof(Set));
  set->elements = (int*) malloc(sizeof(int) * n);
  set->size = 0;
  return set;
}

void set_free(Set* set) {
  free(set->elements);
  free(set);
}

void set_add(Set* s, int v) {
  for (int i = 0; i < s->size; i++)
    if (s->elements[i] == v) return;
  s->elements[s->size++] = v;
}

void set_rem(Set* s, int v) {
  for (int i = 0; i < s->size; i++)
    if (s->elements[i] == v)
      s->elements[i] = s->elements[--s->size];
}

int set_has(Set* s, int v) {
  for (int i = 0; i < s->size; i++)
    if (s->elements[i] == v) return 1;
  return 0;
}

void set_toggle(Set* s, int v) {
  if (set_has(s, v)) set_rem(s, v);
  else               set_add(s, v);
}

#endif
