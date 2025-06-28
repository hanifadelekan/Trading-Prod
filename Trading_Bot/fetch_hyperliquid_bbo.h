#ifndef FETCH_BBO_H
#define FETCH_BBO_H
#include "disruptor.h"
#include <vector>
#include <string>
#include <memory>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>

namespace net = boost::asio;
namespace ssl = boost::asio::ssl;

struct BBOLevel {
    double price;
    double size;
    int num_orders;
    bool is_bid;  // true=bid, false=ask
};

// ✅ The key: define BBOSnapshot struct here!
struct BBOSnapshot {
    std::vector<BBOLevel> levels;
    double midprice;
    double weighted_midprice;
};
extern Disruptor<BBOSnapshot> disruptor;
void run_bbo_async_stream(std::shared_ptr<net::io_context> io_context,
                          net::ssl::context& ssl_ctx,
                          const std::string& symbol, const std::string& channel);

#endif
