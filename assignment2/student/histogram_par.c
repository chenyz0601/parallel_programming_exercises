#include "histogram.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include "names.h"
#define N 10 // number of characters
//unsigned int cur_start; // start point of each job
//unsigned int cur_end; // end point of each job
int global_hist[N]; // for each thread to do reduction
char *shared_buffer; // start pointer of the names
int q_length;
int q_max;
bool all_done;

pthread_mutex_t mutex_variable = PTHREAD_MUTEX_INITIALIZER; // mutex for tasks
pthread_mutex_t mutex_count = PTHREAD_MUTEX_INITIALIZER; // mutex for reduction
struct Job {
    unsigned int start;
    unsigned int end;
};

struct Job queue;

void * Consumer(void * arg) {
    int local_hist[N]; // hold by each thread to count hist
    bool get_my_job = false;
    // initialize
    for (int i = 0; i < N; i ++)
        local_hist[i] = 0;
    struct Job cur_job;
    // local start and end points
    while(1) {
        // lock mutex_variable to get job
        //sem_wait(&full);
        //printf("I am a consumer\n");
        pthread_mutex_lock(&mutex_variable);
        // set the break condition: all jobs are done, then break
        if (all_done) {
            pthread_mutex_unlock(&mutex_variable);
            break;
        }
        if (q_length == 1) {
            get_my_job = true;
            q_length --;
            cur_job = queue;
        }
        pthread_mutex_unlock(&mutex_variable);
            //sleep(0.000001);
        //sem_post(empty);
        // count for local_hist, its job
        if (get_my_job) {
            char current_word[20] = "";
            int c = 0;
            for (unsigned int i = cur_job.start; i < cur_job.end; i++) {
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

void Producer() {
    int count = 0;
    int num_jobs = 0;
    while (num_jobs < 401) {
        pthread_mutex_lock(&mutex_variable);
        if (q_length == 0) {
            q_length ++;
            queue.start = num_jobs * CHUNKSIZE;
            queue.end = (++num_jobs) * CHUNKSIZE;
        }
        pthread_mutex_unlock(&mutex_variable);
    }
    while (shared_buffer[401*CHUNKSIZE+count] != TERMINATOR) {
        count ++;
    }
    pthread_mutex_lock(&mutex_variable);
    queue.start = 401 * CHUNKSIZE;
    queue.end = 401*CHUNKSIZE + count;
    q_length = 1;
    pthread_mutex_unlock(&mutex_variable);
    //printf("producer: last count is: %d\n", count);
    //printf("producer: number of jobs is: %d\n", num_jobs);
    pthread_mutex_lock(&mutex_variable);
    //if (q_length < 0)
    all_done = true;
    //printf("total num jobs is: %d\n", num_jobs);
    pthread_mutex_unlock(&mutex_variable);
}

void get_histogram(char *buffer, int* histogram, int num_threads) {
    //printf("I am called, with nthreads: %d\n", num_threads);
    // initialize all global variables
    //sem_init(full, 0, 0);
    //sem_init(empty, 0, 1);
    all_done = false;
    q_length = 0;
    //q_max = num_threads;
    shared_buffer = buffer;
    //cur_start = 0;
    for (int i = 0; i < N; i ++)
        global_hist[i] = 0;
    // create threads
    pthread_t *thread;
    thread = (pthread_t*)malloc(num_threads * sizeof(*thread));
    //queue = (struct Job*)malloc(num_threads * sizeof(*queue));
    for (int i = 0; i < num_threads; i ++)
        pthread_create(thread+i, NULL, &Consumer, NULL);
    Producer();
    for (int i = 0; i < num_threads; i ++)
        pthread_join(thread[i], NULL);
    // write into histogram
    for (int i = 0; i < N; i ++)
        histogram[i] += global_hist[i];
    free(thread);
}