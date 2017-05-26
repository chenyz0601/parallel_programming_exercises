#include "histogram.h"
#include "names.h"

#include <future>
#include <thread>
#include <vector>
#include <algorithm>

histogram_t& shared_hist;

struct Result {
    int hist[10];
};

void count_hist(promise<struct Result>&& p, vector<word_t>::iterator begin, vector<word_t>::iterator end) {
    for_each(begin, end, [&shared_hist](const word_t word)
    {
        int res = getNameIndex(word.data());
        if (res != -1)
            p.hist[res] ++;
    });
}

void get_histogram(const std::vector<word_t>& words, histogram_t& histogram, int num_threads)
{
    // put your code here
    shared_hist = histogram;
    struct Result result[num_threads];
    for (int i = 0; i < num_threads; i ++)
        for (int j = 0; j < 10; j ++)
            result[i].hist[j] = 0;
    promise<struct Result> *myPromise;
    future<struct Result> *myFuture;
    thread *myThread;
    myPromise = new promise<struct Result>[num_threads];
    myFuture = new future<struct Result>[num_threads];
    myThread = new thread[num_threads];
    unsigned int length = (int)words.size();
}
