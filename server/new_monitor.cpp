#include <iostream>
#include <fstream>
#include <string>
#include <csignal>
#include <atomic>
#include <limits>
#include <iomanip>      // For std::setprecision
#include <thread>       // Required for std::this_thread
#include <chrono>       // Required for std::chrono
#include <map>          // For std::map (sorted by price)
#include <unordered_map> // For std::unordered_map (O(1) average lookup by OrderID)
#include <list>         // For std::list (time priority and O(1) deletion with iterator)
#include <functional>   // For std::greater (to sort bids descending)
#include <optional>     // For std::optional (to safely return best bid/ask)
#include <stdexcept>    // For std::runtime_error

#include "nlohmann/json.hpp" // Include the nlohmann JSON library

// Use the nlohmann::json namespace for convenience
using json = nlohmann::json;

// Price scaling factor (adjust based on minimum tick size)
// Example: if min tick is 0.0001, use 10000. If 0.01, use 100.
const long long PRICE_SCALE_FACTOR = 10000;

// Define types for clarity and to avoid floating-point issues common in HFT
using Price = long long;   // Scaled integer price (e.g., $1.2345 -> 12345)
using OrderID = std::string; // Assuming OrderID comes as a string from JSON
using Size = long long;    // Quantity of an order

// Enum to represent the side of an order
enum class OrderSide {
    BID,
    ASK,
    UNKNOWN // For parsing errors
};

// Convert string side to enum
OrderSide string_to_orderside(const std::string& s) {
    if (s == "B") return OrderSide::BID;
    if (s == "A") return OrderSide::ASK;
    return OrderSide::UNKNOWN;
}

// Convert enum side to string
std::string orderside_to_string(OrderSide side) {
    if (side == OrderSide::BID) return "B";
    if (side == OrderSide::ASK) return "A";
    return "UNKNOWN";
}


// Represents an individual order within the time-priority queue at a price level
struct OrderEntry {
    OrderID id;
    Size size;
    // Potentially add a timestamp for more granular time priority if needed
    // std::chrono::high_resolution_clock::time_point timestamp;
};

// Stores details about an order, including its location in the order book
struct OrderDetails {
    Price price;
    OrderSide side;
    // CRUCIAL: An iterator to the actual OrderEntry in the std::list.
    // This allows O(1) deletion/modification once the OrderID is looked up.
    std::list<OrderEntry>::iterator list_iterator;
};

// The OrderBook class definition
class OrderBook {
public:
    // Bids: Price -> (List of OrderEntry for time priority) -- sorted descending by price
    std::map<Price, std::list<OrderEntry>, std::greater<Price>> bids;

    // Asks: Price -> (List of OrderEntry for time priority) -- sorted ascending by price
    std::map<Price, std::list<OrderEntry>> asks;

    // Global lookup for order details by OrderID.
    // std::unordered_map provides average O(1) lookup.
    std::unordered_map<OrderID, OrderDetails> order_lookup;

    // Real-time aggregated data for O(1) average access
    std::unordered_map<Price, Size> bid_total_sizes;
    std::unordered_map<Price, Size> ask_total_sizes;
    std::unordered_map<Price, long long> bid_order_counts; // Number of unique orders at price
    std::unordered_map<Price, long long> ask_order_counts;

    // Global book totals
    Size total_bid_volume = 0;
    Size total_ask_volume = 0;
    long long total_unique_orders = 0;

    // ===================================================================================
    //                              Order Book Operations
    // ===================================================================================

    void AddOrder(OrderID id, OrderSide side, Price price, Size size) {
        // Basic validation: OrderID should be unique for new orders
        if (order_lookup.count(id)) {
            std::cerr << "Warning: AddOrder called for existing OrderID " << id << ". Ignoring.\n";
            return;
        }
        if (size <= 0) {
            std::cerr << "Warning: AddOrder called with non-positive size for OrderID " << id << ". Ignoring.\n";
            return;
        }

        OrderEntry new_entry = {id, size};
        OrderDetails details;
        details.price = price;
        details.side = side;

        if (side == OrderSide::BID) {
            auto& price_level_list = bids[price]; // Creates entry if price not present
            price_level_list.push_back(new_entry);
            details.list_iterator = std::prev(price_level_list.end()); // Iterator to the newly added element

            bid_order_counts[price]++;
            bid_total_sizes[price] += size;
            total_bid_volume += size;

        } else if (side == OrderSide::ASK) { // OrderSide::ASK
            auto& price_level_list = asks[price]; // Creates entry if price not present
            price_level_list.push_back(new_entry);
            details.list_iterator = std::prev(price_level_list.end()); // Iterator to the newly added element

            ask_order_counts[price]++;
            ask_total_sizes[price] += size;
            total_ask_volume += size;
        } else {
            std::cerr << "Error: AddOrder called with UNKNOWN side for OrderID " << id << ". Ignoring.\n";
            return;
        }

        order_lookup[id] = details;
        total_unique_orders++;
        // std::cout << "[OrderBook] Added OrderID: " << id << " Side: " << orderside_to_string(side) << " Price: " << static_cast<double>(price)/PRICE_SCALE_FACTOR << " Size: " << size << std::endl;
    }

    // Handles full or partial cancellations
    bool CancelOrder(OrderID id) {
        auto it_lookup = order_lookup.find(id);
        if (it_lookup == order_lookup.end()) {
            // std::cerr << "Warning: Attempted to cancel non-existent OrderID " << id << std::endl;
            return false; // Order not found
        }

        OrderDetails details = it_lookup->second;
        Price price = details.price;
        OrderSide side = details.side;
        Size order_size_to_remove = details.list_iterator->size; // Get current size from the list entry

        if (side == OrderSide::BID) {
            auto it_price_level = bids.find(price);
            if (it_price_level != bids.end()) {
                it_price_level->second.erase(details.list_iterator); // O(1) deletion

                bid_order_counts[price]--;
                bid_total_sizes[price] -= order_size_to_remove;
                total_bid_volume -= order_size_to_remove;

                if (it_price_level->second.empty()) {
                    bids.erase(it_price_level);
                    bid_order_counts.erase(price);
                    bid_total_sizes.erase(price);
                }
            }
        } else if (side == OrderSide::ASK) { // OrderSide::ASK
            auto it_price_level = asks.find(price);
            if (it_price_level != asks.end()) {
                it_price_level->second.erase(details.list_iterator); // O(1) deletion

                ask_order_counts[price]--;
                ask_total_sizes[price] -= order_size_to_remove;
                total_ask_volume -= order_size_to_remove;

                if (it_price_level->second.empty()) {
                    asks.erase(it_price_level);
                    ask_order_counts.erase(price);
                    ask_total_sizes.erase(price);
                }
            }
        } else {
            std::cerr << "Error: CancelOrder called with UNKNOWN side for OrderID " << id << ". Ignoring.\n";
            return false;
        }

        order_lookup.erase(it_lookup); // O(1) average deletion
        total_unique_orders--;
        // std::cout << "[OrderBook] Canceled OrderID: " << id << std::endl;
        return true;
    }

    // Handles partial fills or size modifications
    void UpdateOrder(OrderID id, Size new_size) {
        auto it_lookup = order_lookup.find(id);
        if (it_lookup == order_lookup.end()) {
            // std::cerr << "Warning: Attempted to update non-existent OrderID " << id << std::endl;
            return; // Order not found
        }
        if (new_size < 0) {
            std::cerr << "Warning: UpdateOrder called with negative new_size for OrderID " << id << ". Ignoring.\n";
            return;
        }

        OrderDetails& details = it_lookup->second; // Get a reference to modify
        Size old_size = details.list_iterator->size;

        // If new_size is 0, treat as a cancellation
        if (new_size == 0) {
            CancelOrder(id);
            return;
        }

        // Update the size in the actual order entry within the list
        details.list_iterator->size = new_size;

        // Adjust aggregated sizes
        Size size_diff = new_size - old_size;

        if (details.side == OrderSide::BID) {
            bid_total_sizes[details.price] += size_diff;
            total_bid_volume += size_diff;
        } else if (details.side == OrderSide::ASK) { // OrderSide::ASK
            ask_total_sizes[details.price] += size_diff;
            total_ask_volume += size_diff;
        } else {
            std::cerr << "Error: UpdateOrder called with UNKNOWN side for OrderID " << id << ". Ignoring.\n";
            return;
        }
        // std::cout << "[OrderBook] Updated OrderID: " << id << " New Size: " << new_size << std::endl;
    }

    // ===================================================================================
    //                              Query Methods
    // ===================================================================================

    std::optional<Price> GetBestBidPrice() const {
        if (bids.empty()) {
            return std::nullopt;
        }
        return bids.begin()->first;
    }

    std::optional<Price> GetBestAskPrice() const {
        if (asks.empty()) {
            return std::nullopt;
        }
        return asks.begin()->first;
    }

    Size GetTotalBidSizeAtPrice(Price price) const {
        auto it = bid_total_sizes.find(price);
        if (it != bid_total_sizes.end()) {
            return it->second;
        }
        return 0;
    }

    Size GetTotalAskSizeAtPrice(Price price) const {
        auto it = ask_total_sizes.find(price);
        if (it != ask_total_sizes.end()) {
            return it->second;
        }
        return 0;
    }

    // For debugging/visualization
    void PrintOrderBook() const {
        std::cout << "\n--- Order Book Snapshot ---\n";
        std::cout << "Total Unique Orders: " << total_unique_orders << "\n";

        // Asks
        std::cout << "--- ASKS (Total Volume: " << total_ask_volume << ") ---\n";
        if (asks.empty()) {
            std::cout << "  <EMPTY>\n";
        } else {
            for (const auto& pair : asks) {
                Price price = pair.first;
                const auto& orders_at_price = pair.second;
                Size total_size_at_level = ask_total_sizes.at(price); // Use pre-calculated sum
                long long order_count_at_level = ask_order_counts.at(price); // Use pre-calculated count

                std::cout << std::fixed << std::setprecision(4)
                          << "  Price: " << static_cast<double>(price) / PRICE_SCALE_FACTOR
                          << " | Count: " << order_count_at_level
                          << " | Total Size: " << total_size_at_level << "\n";
                // Uncomment below to see individual orders at each level
                /*
                for (const auto& order_entry : orders_at_price) {
                    std::cout << "    - OID: " << order_entry.id << ", Size: " << order_entry.size << "\n";
                }
                */
            }
        }

        std::cout << "--------------------------\n";

        // Bids
        std::cout << "--- BIDS (Total Volume: " << total_bid_volume << ") ---\n";
        if (bids.empty()) {
            std::cout << "  <EMPTY>\n";
        } else {
            for (const auto& pair : bids) {
                Price price = pair.first;
                const auto& orders_at_price = pair.second;
                Size total_size_at_level = bid_total_sizes.at(price); // Use pre-calculated sum
                long long order_count_at_level = bid_order_counts.at(price); // Use pre-calculated count

                std::cout << std::fixed << std::setprecision(4)
                          << "  Price: " << static_cast<double>(price) / PRICE_SCALE_FACTOR
                          << " | Count: " << order_count_at_level
                          << " | Total Size: " << total_size_at_level << "\n";
                // Uncomment below to see individual orders at each level
                /*
                for (const auto& order_entry : orders_at_price) {
                    std::cout << "    - OID: " << order_entry.id << ", Size: " << order_entry.size << "\n";
                }
                */
            }
        }
        std::cout << "--------------------------\n";
    }
};


// Global flag to control the main loop, allowing graceful shutdown
std::atomic<bool> keep_running(true);

// Signal handler for Ctrl+C (SIGINT) and termination signals (SIGTERM)
void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        keep_running = false;
        std::cerr << "\n[Monitor] Shutdown signal received. Exiting gracefully...\n";
    }
}

int main(int argc, char* argv[]) {
    // Disable buffering to ensure immediate output
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::cout << std::unitbuf;

    // Check for correct command-line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filepath>\n";
        return 1;
    }

    const std::string filepath = argv[1];

    // Register signal handlers for graceful shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // Open the file for reading
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "[Monitor] Error opening file: " << filepath << std::endl;
        return 1;
    }

    std::cout << "[Monitor] Watching: " << filepath << std::endl;

    // Instantiate the OrderBook
    OrderBook order_book;

    // Set precision for floating-point output
    std::cout << std::fixed << std::setprecision(4);

    // Main loop to read and process the file
    while (keep_running) {
        std::string line;

        // Try reading a new line from the file
        if (std::getline(file, line)) {
            if (line.empty()) {
                continue; // Skip empty lines
            }

            try {
                // Parse the line as a JSON object
                json data = json::parse(line);

                // --- Extract required fields ---
                std::string side_str = data.at("side").get<std::string>();
                OrderSide side = string_to_orderside(side_str);
                double price_double = std::stod(data.at("px").get<std::string>());
                Price price = static_cast<Price>(price_double * PRICE_SCALE_FACTOR);
                
                std::string action = "unknown";
                OrderID order_id = "";
                Size size = 0; // Initialize size for safety

                // The 'raw_book_diff' object contains the action type ('new', 'modify', 'delete')
                if (data.contains("raw_book_diff")) {
                    json diff = data.at("raw_book_diff");
                    
                    if (diff.contains("new")) {
                        action = "new";
                        order_id = diff.at("new").at("oid").get<std::string>();
                        size = std::stod(diff.at("new").at("sz").get<std::string>());
                        order_book.AddOrder(order_id, side, price, size);

                    } else if (diff.contains("update")) {
                        action = "update";
                        order_id = diff.at("update").at("oid").get<std::string>();
                        size = std::stod(diff.at("update").at("sz").get<std::string>());
                        order_book.UpdateOrder(order_id, size);

                    } else if (diff.contains("remove")) {
                        action = "remove";
                        order_id = diff.at("remove").at("oid").get<std::string>();
                        // Size might not be present in a delete action, but we don't need it
                        // for CancelOrder as it retrieves the size from internal lookup.
                        order_book.CancelOrder(order_id);
                    }
                }
                
                // --- Print the parsed data and current best bid/ask ---
                std::cout << "[ACTION]: " << action 
                          << " | [SIDE]: " << side_str 
                          << " | [PRICE]: " << price_double // Print original double for readability
                          << " | [OID]: " << order_id
                          << " | [SIZE]: " << size; // Size as received in diff, not necessarily current book size

                // Get and print current best bid/ask from the order book
                std::optional<Price> best_bid_price = order_book.GetBestBidPrice();
                std::optional<Price> best_ask_price = order_book.GetBestAskPrice();

                std::cout << " | Best Bid: " << (best_bid_price ? static_cast<double>(*best_bid_price)/PRICE_SCALE_FACTOR : 0.0)
                          << " | Best Ask: " << (best_ask_price ? static_cast<double>(*best_ask_price)/PRICE_SCALE_FACTOR : std::numeric_limits<double>::max())
                          << std::endl;

                // Uncomment the line below to print the full order book after each update
                // order_book.PrintOrderBook();


            } catch (const json::parse_error& e) {
                // This will catch lines that are not valid JSON
                std::cerr << "[Monitor] JSON parse error: " << e.what() << " on line: " << line << std::endl;
            } catch (const std::out_of_range& e) {
                // Catches errors if 'at()' fails due to missing keys
                std::cerr << "[Monitor] Data extraction error (missing key): " << e.what() << " on line: " << line << std::endl;
            } catch (const std::exception& e) {
                // This will catch other errors, e.g., conversion errors
                std::cerr << "[Monitor] General data processing error: " << e.what() << " on line: " << line << std::endl;
            }

        } else {
            // No new line available, check for end-of-file
            if (file.eof()) {
                file.clear(); // Clear EOF flag to allow further reads
            }
            // A short sleep to prevent the loop from spinning too fast and using 100% CPU
            // when there's no new data.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    file.close();
    std::cout << "[Monitor] File monitoring finished. Final Order Book state:\n";
    order_book.PrintOrderBook(); // Print final state
    return 0;
}