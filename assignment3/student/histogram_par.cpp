#include "histogram.h"
#include "names.h"

#include <future>
#include <vector>
#include <algorithm>

histogram_t count_hist(const std::vector<word_t>& words, int b, int e) {
    histogram_t local_hist = {{0}};
    for_each(begin(words)+b, begin(words)+e, [&local_hist](const word_t& word)
    {
        int res = getNameIndex(word.data());
        if (res != -1) local_hist[res]++;
    });
    return local_hist;
}

void get_histogram(const std::vector<word_t>& words, histogram_t& histogram, int num_threads) {
    // put your code here
    std::vector<std::future<histogram_t>> myFutures;
    int chunk_size = words.size()/num_threads;
    int res = words.size()%num_threads;
    for (int i = 0; i < chunk_size*(num_threads-1); i += chunk_size)
        myFutures.push_back(std::async(std::launch::async, count_hist, std::ref(words), i, i+chunk_size));
    myFutures.push_back(std::async(std::launch::async, count_hist, std::ref(words), (num_threads-1)*chunk_size, res+num_threads*chunk_size));
    for (auto& f : myFutures) {
        histogram_t temp_hist = f.get();
        for (int j = 0; j < NNAMES; j ++)
            histogram[j] += temp_hist[j];
    }
}
