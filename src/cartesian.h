#ifndef CARTESIAN_H
#define CARTESIAN_H

#include <math.h>

typedef struct {
  int x, y;
} Point;

double distance(Point a, Point b) {
  return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

#endif
