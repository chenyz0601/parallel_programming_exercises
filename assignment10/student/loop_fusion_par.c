#include<omp.h>
void compute(unsigned long **a, unsigned long **b, unsigned long **c, unsigned long **d, int N, int num_threads) {

	// perform loop fusion to transform this loop and parallelize it with OpenMP
#pragma omp parallel
  {
#pragma omp for
	for (int i = 1; i < N; i++) {
		for (int j = 1; j < N; j++) {
			a[i][j] = 2 * b[i][j];
			d[i][j] = a[i][j] * c[i][j];
		}
		for (int j = 1; j < N; i++) {
			c[i][j - 1] = a[i][j - 1] - a[i][j + 1];
		}
	}
  }
}
