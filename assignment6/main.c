#include "ds.h"
#include "companytree.h"
#include "vis.h"
#include "time.h"

int main(int argc, char** argv){
  long int num_threads = 4;
  struct timespec begin, end;

  // argument handling
  if (argc != 2) {
    fprintf(stderr, "usage: %s [#threads]\n", argv[0]);
    return 1;
  }
  if ((num_threads = strtol(argv[1], NULL, 0)) == 0 || num_threads < 0) {
    fprintf(stderr, "#threads not valid!\n");
    return 1;
  }

  // insert nodes (employee positions) in the company tree
  tree *topNode = malloc(sizeof(tree));
  initialize(topNode);


  // visit all of them to compute work hours in a mysterious way
  clock_gettime(CLOCK_MONOTONIC, &begin);
  traverse(topNode, num_threads);
  clock_gettime(CLOCK_MONOTONIC, &end);
  
  // Showing results
  visualize();

  tearDown(topNode);
  
  // print timing information
  printf("\n\nTime: %.5f seconds\n", ((double)end.tv_sec + 1.0e-9*end.tv_nsec) -
                     ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec));
  return 0;
}

