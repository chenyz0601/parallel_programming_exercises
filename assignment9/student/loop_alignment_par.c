#include<omp.h>


void compute(unsigned long **a, unsigned long **b, unsigned long **c, unsigned long **d, int N, int num_threads) {

	// perform loop alignment to transform this loop and parallelize it with OpenMP
	for (int i = 1; i < N; i++) { 
#pragma omp parallel for shared(i)
		for (int j = 0; j < N+1; j++) {
      if (j < N && j > 0)
			  a[i][j] = 3 * b[i][j];
      if (j > 1)
			  b[i][j] = c[i][j-1] * c[i][j-1];
      if (j < N-1)
			  c[i][j] = a[i][j+1] * d[i][j+1];
		}
	}
}
