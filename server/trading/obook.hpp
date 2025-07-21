#pragma once
#include "disruptor.h"
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

struct OBSnapshot {
    double imb;
    double timestamp;
};

// Function to run the order book async stream
// Now uses the aliased namespaces
void run_orderbook_async_stream(std::shared_ptr<net::io_context> io_context,
                                  net::ssl::context& ssl_ctx, // Changed to net::ssl::context
                                  const std::string& symbol);

class HyperliquidOBParser {
public:
    OrderBookSnapshot parse(std::string_view json) {
        OrderBookSnapshot snapshot;

        // Find the "levels" key and its array content
        size_t levels_key_pos = json.find("\"levels\":");
        if (levels_key_pos == std::string::npos) {
            // This is the expected behavior for non-l2Book messages like subscription responses.
            // Do not throw, just return empty snapshot.
            return snapshot;
        }
        
        // Correctly find the start of the outer levels array, which is "[[", not just "["
        // The json looks like: "levels":[[...],[...]]
        size_t outer_arr_start_delimiter = json.find("[[", levels_key_pos);
        if (outer_arr_start_delimiter == std::string::npos) {
            throw std::runtime_error("Malformed levels array: missing '[[' after \"levels\":");
        }

        // Now, find the corresponding closing ']]' for the levels array
        // It's crucial to search from after the levels_key_pos.
        // We need to find the ']]' that closes the outer levels array, not an inner one.
        // A simple find(']]') from outer_arr_start_delimiter might not be robust for all JSON.
        // Let's assume the structure is always "levels":[[...],[...]] and we need the final "]]"
        // within the scope of the "data" object.
        // A simpler way: find the "],[", then find the next "]]".

        size_t bids_asks_separator = json.find("],[", outer_arr_start_delimiter);
        if (bids_asks_separator == std::string::npos) {
            throw std::runtime_error("Malformed levels array: missing bid/ask separator '],['");
        }

        // The end of the overall levels array (the ']]' that closes it)
        size_t outer_arr_end_delimiter = json.find("]]", bids_asks_separator);
        if (outer_arr_end_delimiter == std::string::npos) {
            throw std::runtime_error("Malformed levels array: missing outer ']]'");
        }

        // Now, extract the content that contains both bid and ask arrays: "[{"px":...}], [{"px":...}]"
        // This content starts AFTER the first '[[' and ends BEFORE the final ']]'.
        std::string_view full_levels_content(json.data() + outer_arr_start_delimiter + 2,
                                             outer_arr_end_delimiter - (outer_arr_start_delimiter + 2));

        // The separator found earlier (bids_asks_separator) was in the full JSON.
        // We need its position relative to full_levels_content.
        size_t relative_separator_pos = full_levels_content.find("],[");
        if (relative_separator_pos == std::string::npos) {
             throw std::runtime_error("Internal error: bid/ask separator not found in extracted levels content.");
        }


        std::string_view bids_array_content = full_levels_content.substr(0, relative_separator_pos);
        std::string_view asks_array_content = full_levels_content.substr(relative_separator_pos + 3);


        // Helper lambda for parsing individual level objects
        auto parse_levels_from_array_content = [&](std::string_view array_content_sv, bool is_bid) {
            size_t current_pos = 0;
            // The array_content_sv now contains only objects, like {"px":...},{"px":...}
            // No need to trim leading/trailing brackets, as they are handled by `full_levels_content` extraction.

            while (current_pos < array_content_sv.size()) {
                size_t obj_start = array_content_sv.find('{', current_pos);
                if (obj_start == std::string::npos) break; // No more objects

                size_t obj_end = array_content_sv.find('}', obj_start);
                if (obj_end == std::string::npos) {
                    throw std::runtime_error("Malformed level object (missing '}')");
                }

                std::string_view level_json(array_content_sv.data() + obj_start, obj_end - obj_start + 1);

                OBLevelUpdate update;
                update.action = OBUpdateAction::Insert;
                update.is_bid = is_bid;

                // Robust price extraction
                size_t p_pos = level_json.find("\"px\":\"");
                if (p_pos == std::string::npos)
                    throw std::runtime_error("Price missing in level");
                size_t p_value_start_idx = p_pos + 6; // Length of "\"px\":\""
                size_t p_value_end_idx = level_json.find('\"', p_value_start_idx);
                if (p_value_end_idx == std::string::npos)
                    throw std::runtime_error("Malformed price field: missing closing quote");
                
                // Using string_view::substr directly to avoid large string copy before stod
                std::string_view price_str = level_json.substr(p_value_start_idx, p_value_end_idx - p_value_start_idx);
                update.price = std::stod(std::string(price_str)); // Still need std::string for stod

                // Robust size extraction
                size_t s_pos = level_json.find("\"sz\":\"");
                if (s_pos == std::string::npos)
                    throw std::runtime_error("Size missing in level");
                size_t s_value_start_idx = s_pos + 6; // Length of "\"sz\":\""
                size_t s_value_end_idx = level_json.find('\"', s_value_start_idx);
                if (s_value_end_idx == std::string::npos)
                    throw std::runtime_error("Malformed size field: missing closing quote");

                // Using string_view::substr directly to avoid large string copy before stod
                std::string_view size_str = level_json.substr(s_value_start_idx, s_value_end_idx - s_value_start_idx);
                update.size = std::stod(std::string(size_str)); // Still need std::string for stod

                snapshot.updates.push_back(update);

                current_pos = obj_end + 1; // Move past the current object
            }
        };

        // Parse bids
        parse_levels_from_array_content(bids_array_content, true);
        // Parse asks
        parse_levels_from_array_content(asks_array_content, false);

        return snapshot;
    }
};