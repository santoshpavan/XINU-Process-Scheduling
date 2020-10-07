/* Implementations for math.h */
#include <lab1.h>

#define MAX_TAYLOR_ITER 20
#define RAND_MAX 077777

/* pow implementation */
double pow(double x, int y) {
  // x ^ y
  double powval = 1;
    for (; y != 0; y--) {
      powval *= x;
    }
  return powval;
}

/* natural logarithm calculation */
double log(double x) {
  //Taylor series
  double logval = 0;
  int i = 1;
  for (; i <= MAX_TAYLOR_ITER; i++) {
    logval += pow(-1, i + 1) * pow(x - 1, i) * (1/(double)i);
  }
  return logval;
}

/* exponential distribution */
double expdev(double lambda) {
  double randval;
  do {
    randval = (double)rand() / RAND_MAX;
  } while (randval == 0);
  return -log(randval) / lambda;
}