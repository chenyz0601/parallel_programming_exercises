#include "histogram.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "names.h"
#define N 10 // number of characters
unsigned int cur_start; // start point of each job
unsigned int cur_end; // end point of each job
int global_hist[N]; // for each thread to do reduction
char *shared_buffer; // start pointer of the names
int q_length = 0;
bool all_done = false;

pthread_mutex_t mutex_variable = PTHREAD_MUTEX_INITIALIZER; // mutex for tasks
pthread_mutex_t mutex_count = PTHREAD_MUTEX_INITIALIZER; // mutex for reduction

void * Consumer(void * arg) {
    int local_hist[N]; // hold by each thread to count hist
    bool get_my_job = false;
    // initialize
    for (int i = 0; i < N; i ++)
        local_hist[i] = 0;
    // local start and end points
    unsigned int start, end;
    while(1) {
        // lock mutex_variable to get job
        pthread_mutex_lock(&mutex_variable);
        // set the break condition: all jobs are done, then break
        if (all_done)
            break;
        if (q_length == 1) {
            start = cur_start; // get current start point
            end = cur_end;
            get_my_job = true;
            q_length = 0;
        }
        else
            pthread_barrier_wait(0.00001ms);
        pthread_mutex_unlock(&mutex_variable);
        // count for local_hist, its job
        if (get_my_job) {
            char current_word[20] = "";
            int c = 0;
            for (unsigned int i = start; i < end; i++) {
                if(isalpha(shared_buffer[i])){
                    current_word[c++] = shared_buffer[i];
                } else {
                    current_word[c] = '\0';
                    int res = getNameIndex(current_word);
                    if (res != -1)
                        local_hist[res]++;
                    c = 0;
                }
            }
            get_my_job = false;
        }
    }
    // when global_end is reached, do the reduction
    pthread_mutex_lock(&mutex_count);
    for (int i = 0; i < N; i ++)
        global_hist[i] += local_hist[i];
    pthread_mutex_unlock(&mutex_count);
    return NULL;
}

void * Producer(void * ptr) {
    unsigned int count = 0;
    int sub_count = 0;
    while (shared_buffer[count] != TERMINATOR) {
        // create a job
        pthread_mutex_lock(&mutex_variable);
        if (count%CHUNKSIZE == 0 && q_length == 0) {
            cur_start = count;
            sub_count = 1;
            while (sub_count != CHUNKSIZE || shared_buffer[cur_start+sub_count] != TERMINATOR)
                sub_count ++;
            cur_end = cur_start + sub_count;
        }
        q_length = 1;
        pthread_mutex_unlock(&mutex_variable);
        count += sub_count;
    }
    pthread_mutex_lock(&mutex_variable);
    all_done = true;
    pthread_mutex_unlock(&mutex_variable);
    return NULL;
}

void get_histogram(char *buffer, int* histogram, int num_threads) {
    // initialize all global variables
    shared_buffer = buffer;
    cur_start = 0;
    for (int i = 0; i < N; i ++)
        global_hist[i] = 0;
    // create threads
    pthread_t *thread;
    thread = (pthread_t*)malloc((num_threads-1) * sizeof(*thread));
    pthread_create(thread, NULL, &Producer, NULL);
    for (int i = 1; i < num_threads; i ++)
        pthread_create(thread+i, NULL, &Consumer, NULL);
    for (int i = 0; i < num_threads; i ++)
        pthread_join(thread[i], NULL);
    // write into histogram
    for (int i = 0; i < N; i ++)
        histogram[i] += global_hist[i];
    free(thread);
}