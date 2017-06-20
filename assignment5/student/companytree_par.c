#include "companytree.h"

#define T 30

void traverse_util(tree *node, int numThreads, int count);

void traverse(tree *node, int numThreads){

//write your parallel solution here

  int global_count = 0;

#pragma omp parallel num_threads(numThreads)
  {
#pragma omp single
  traverse_util(node, numThreads, global_count);
  }
}

void traverse_util(tree *node, int numThreads, int count) {

	if(node != NULL){

      count ++;
		
#pragma omp task shared(count) firstprivate(node, numThreads) final(count > T)
		  traverse_util(node->right, numThreads, count);
#pragma omp task shared(count) firstprivate(node, numThreads) final(count > T)
		  traverse_util(node->left, numThreads, count);
#pragma omp taskwait
      {
		    node->work_hours = compute_workHours(node->data);
		    top_work_hours[node->id] = node->work_hours;
      }
  }
}

