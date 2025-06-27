#ifndef FETCH_BBO_H
#define FETCH_BBO_H

#include <vector>
#include <string>
#include <functional>

struct BBOLevel {
    double price;
    double size;
    int num_orders;
};

// Declaration: streaming BBO feed calls the handler on each update
void fetch_bbo_stream(const std::string& symbol,
                      const std::string& channel,
                      const std::function<void(const std::vector<BBOLevel>&)>& on_bbo);

#endif
