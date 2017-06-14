#include "histogram.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include "names.h"

unsigned int last = 0, countable;
histogram_t hist = {0};
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void * consumer(void *buffer_ptr) {
    char *buffer = (char *)buffer_ptr;
    histogram_t local_hist = {0};
    unsigned int c, start, i;
    char *b;
    while (countable > 7483646) {
        char current_word[20] = "";
        c = 0;
        pthread_mutex_lock(&mutex1);
        start = last;
        b = buffer+start;
        for (i = 0; i < CHUNKSIZE; i ++) {
            if (b[i] == TERMINATOR)
                countable = start;
        }
        last += CHUNKSIZE;
        pthread_mutex_unlock(&mutex1);
        if (start >= countable) {
            pthread_mutex_lock(&mutex2);
            for (int k = 0; k < 10; k ++)
                hist[k] += local_hist[k];
            pthread_mutex_unlock(&mutex2);
            return NULL;
        }
        for (i = 0; i < CHUNKSIZE; i ++) {
            if (isalpha(b[i]))
                current_word[c++] = b[i];
            else {
                current_word[c] = '\0';
                int res = getNameIndex(current_word);
                if (res != -1)
                    local_hist[res] ++;
                c = 0;
            }
        }
    }
    return NULL;
}

void get_histogram(char *buffer, int* histogram, int num_threads) {
    countable = 47483647;
    pthread_t *thread = (pthread_t*)malloc(num_threads*sizeof(*thread));
    for (int i = 0; i < num_threads; i ++)
        pthread_create(thread+i, NULL, &consumer, buffer);
    for (int i = 0; i < num_threads; i ++)
        pthread_join(thread[i], NULL);
    for (int k = 0; k < 10; k ++)
        histogram[k] = hist[k];
    free(thread);
}
/*
#define N 10 // number of characters
int *global_hist; // for each thread to do reduction
char *shared_buffer; // start pointer of the names
int residual;
int job_index;

pthread_mutex_t mutex_variable = PTHREAD_MUTEX_INITIALIZER; // mutex for tasks
pthread_mutex_t mutex_count = PTHREAD_MUTEX_INITIALIZER; // mutex for reduction

void * Consumer(void * arg) {
    int local_hist[N]; // hold by each thread to count hist
    //bool get_my_job = false;
    // initialize
    for (int i = 0; i < N; i ++)
        local_hist[i] = 0;
    int count = 0;
    unsigned int start, end;
    // local start and end points
    while(1) {
        pthread_mutex_lock(&mutex_variable);
        count = job_index;
        job_index++;
        pthread_mutex_unlock(&mutex_variable);
        if (count < 401) {
            start = count * CHUNKSIZE;
            end = (count + 1) * CHUNKSIZE;
        } else if (count == 401) {
            start = count * CHUNKSIZE;
            end = count * CHUNKSIZE + residual;
        } else
            break;
        char current_word[20] = "";
        int c = 0;
        for (unsigned int i = start; i < end; i++) {
            if (isalpha(shared_buffer[i])) {
                current_word[c++] = shared_buffer[i];
            } else {
                current_word[c] = '\0';
                int res = getNameIndex(current_word);
                if (res != -1)
                    local_hist[res]++;
                c = 0;
            }
        }
    }
    pthread_mutex_lock(&mutex_count);
    for (int i = 0; i < N; i ++)
        global_hist[i] += local_hist[i];
    pthread_mutex_unlock(&mutex_count);
    return NULL;
}

void Producer() {
    int my_res = 0;
    while (shared_buffer[401*CHUNKSIZE+my_res] != TERMINATOR)
        my_res ++;
    residual = my_res;
}

void get_histogram(char *buffer, int* histogram, int num_threads) {
    shared_buffer = buffer;
    job_index = 0;
    global_hist = histogram;
    pthread_t *thread;
    thread = (pthread_t*)malloc(num_threads * sizeof(*thread));
    for (int i = 0; i < num_threads; i ++) {
        pthread_create(thread+i, NULL, &Consumer, NULL);
    }
    Producer();
    for (int i = 0; i < num_threads; i ++)
        pthread_join(thread[i], NULL);
    free(thread);
}
*/