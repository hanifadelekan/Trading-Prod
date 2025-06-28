#ifndef FETCH_BBO_H
#define FETCH_BBO_H

#include <vector>
#include <string>
#include <deque>
#include <mutex>

struct BBOLevel {
    double price;
    double size;
    int num_orders;
};

void run_bbo_async_stream(const std::string& symbol, const std::string& channel);

// Shared queue declaration
extern std::deque<std::vector<BBOLevel>> bbo_queue;

// Shared mutex declaration
extern std::mutex bbo_mutex;

#endif
