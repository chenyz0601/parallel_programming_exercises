#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "histogram.h"

struct pthread_args {
    int curBlock;
    block_t *blocks;
    int num_curBlock;
    int hist_count[NNAMES];
};

void * compute_histogram(void * ptr) {
    struct pthread_args *arg = (struct pthread_args*)ptr;
    char current_word[20] = "";
    int c = 0;
    for (int i = arg->curBlock; i < arg->num_curBlock; i ++) { 
      for (int j = 0; j < BLOCKSIZE; j ++) {
         if (isalpha(arg->blocks[i][j]))
              current_word[c++] = arg->blocks[i][j];
          else {
              current_word[c] = '\0';
              for (int k = 0; k < NNAMES; k ++) {
                  if (!strcmp(current_word, names[k]))
                      arg->hist_count[k] ++;
              }
              c = 0;
          }
       }
    }
    return NULL;
}

void get_histogram(int nBlocks, block_t *blocks, histogram_t histogram, int num_threads) {
	// write your parallel solution here
    struct pthread_args * arg;
    pthread_t *thread;
    thread = (pthread_t *)malloc(num_threads * sizeof(*thread));
    arg = (struct pthread_args *)malloc(num_threads * sizeof(*arg));
    for (int i = 0; i < num_threads; i ++) {
      for (int k = 0; k < NNAMES; k ++) {
        arg[i].hist_count[k] = 0;
      }
    }
    int res = nBlocks % num_threads;
    int num_thread_group = (nBlocks-res)/num_threads;
    for (int i = 0; i < num_threads; i ++) {
        arg[i].curBlock = i * num_thread_group;
        arg[i].blocks = blocks;
        arg[i].num_curBlock = (i+1)*num_thread_group;
        pthread_create(thread+i, NULL, &compute_histogram, arg+i);
    }
    for (int j = 0; j < num_threads; j ++)
        pthread_join(thread[j], NULL);
    for (int i = 0; i < num_threads; i ++) {
      for (int k = 0; k < NNAMES; k ++) {
        histogram[k] += arg[i].hist_count[k];
      }
    }
    free(thread);
    free(arg);
}
