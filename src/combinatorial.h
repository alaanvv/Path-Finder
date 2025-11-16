#ifndef COMBINATORIAL_H
#define COMBINATORIAL_H

int factorial(int k) {
  if (k < 2) return 1;
  return k * factorial(k - 1);
}

int permutations(int n, int r) {
  if (r > n) return 0;
  return factorial(n) / factorial(n - r);
}

int combinations(int n, int r) {
  if (r > n) return 0;
  return factorial(n) / (factorial(r) * factorial(n - r));
}

#endif
