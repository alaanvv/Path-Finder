#ifndef COMBINATORIAL_H
#define COMBINATORIAL_H

int factorial(int k) {
  if (k < 2) return 1;
  return k * factorial(k - 1);
}

int permutations(int n, int r) {
  if (r > n) return 0;
  int p = 1;
  for (int i = 0; i < r; i++)
    p *= (n - i);
  return p;
}

int combinations(int n, int r) {
  if (r > n) return 0;
  int num = 1, den = 1;
  for (int i = 0; i < r; i++) {
    num *= (n - i);
    den *= (i + 1);
  }
  return num / den;
}

#endif

