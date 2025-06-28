#pragma once

#include <vector>
#include <string> // Added for std::string
#include <memory>   // Added for std::shared_ptr

// ADD THESE BOOST INCLUDES AND NAMESPACE ALIASES
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>

// Optionally, you can add these namespace aliases here or in the .cpp file
// It's generally good practice to put 'using namespace' in .cpp files,
// but for type declarations in headers, you might alias specific components.
// For now, let's use the full boost::asio:: for clarity, or just
// bring in the specific aliases needed for the function signature.
// For simplicity, let's alias net and ssl specifically for the function signature.
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;


enum class OBUpdateAction {
    Insert,
    Modify,
    Delete
};

struct OBLevelUpdate {
    OBUpdateAction action;
    double price;
    double size;
    bool is_bid; // true=bid, false=ask
};

struct OrderBookSnapshot {
    std::vector<OBLevelUpdate> updates; // multiple updates per message
};

// Function to run the order book async stream
// Now uses the aliased namespaces
void run_orderbook_async_stream(std::shared_ptr<net::io_context> io_context,
                                  net::ssl::context& ssl_ctx, // Changed to net::ssl::context
                                  const std::string& symbol);