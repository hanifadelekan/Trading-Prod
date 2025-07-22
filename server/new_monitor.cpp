#include <iostream>
#include <fstream>
#include <string>
#include <csignal>
#include <atomic>
#include <limits>
#include <iomanip> // For std::setprecision
#include "nlohmann/json.hpp" // Include the nlohmann JSON library

// Use the nlohmann::json namespace for convenience
using json = nlohmann::json;

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

    // Initialize variables to track the best bid and ask
    double highest_bid = 0.0;
    double lowest_ask = std::numeric_limits<double>::max();

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
                std::string side = data["side"];
                double price = std::stod(data["px"].get<std::string>());
                
                std::string action = "unknown";
                double size = 0.0;

                // The 'raw_book_diff' object contains the action type ('new', 'modify', 'delete')
                if (data.contains("raw_book_diff")) {
                    json diff = data["raw_book_diff"];
                    if (diff.contains("new")) {
                        action = "new";
                        size = std::stod(diff["new"]["sz"].get<std::string>());
                    } else if (diff.contains("modify")) {
                        action = "modify";
                        size = std::stod(diff["modify"]["sz"].get<std::string>());
                    } else if (diff.contains("delete")) {
                        action = "delete";
                        // Size might not be present in a delete action, default to 0
                        if (diff["delete"].contains("sz")) {
                           size = std::stod(diff["delete"]["sz"].get<std::string>());
                        }
                    }
                }
                
                // --- Print the parsed data ---
                std::cout << "[ACTION]: " << action 
                          << " | [SIDE]: " << side 
                          << " | [PRICE]: " << price 
                          << " | [SIZE]: " << size << std::endl;

                // --- Update and track the highest bid and lowest ask ---
                if (side == "Bid") {
                    if (price > highest_bid) {
                        highest_bid = price;
                        std::cout << "  -> New Highest Bid: " << highest_bid << std::endl;
                    }
                } else if (side == "Ask") {
                    if (price < lowest_ask) {
                        lowest_ask = price;
                        std::cout << "  -> New Lowest Ask: " << lowest_ask << std::endl;
                    }
                }

            } catch (const json::parse_error& e) {
                // This will catch lines that are not valid JSON
                // std::cerr << "[Monitor] JSON parse error: " << e.what() << " on line: " << line << std::endl;
            } catch (const std::exception& e) {
                // This will catch other errors, e.g., missing keys or conversion errors
                // std::cerr << "[Monitor] Data processing error: " << e.what() << " on line: " << line << std::endl;
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
    return 0;
}
