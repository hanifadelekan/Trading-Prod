#include <iostream>
#include <fstream>
#include <string>
#include <csignal>
#include <atomic>
#include <limits>
#include <iomanip>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include "uWS.h"
#include "nlohmann/json.hpp"

// Use namespaces for convenience
using json = nlohmann::json;

// --- Global Variables ---

// Atomic flag for graceful shutdown across threads
std::atomic<bool> keep_running(true);

// WebSocket server group to manage all connected clients
uWS::Group<uWS::SERVER>* ws_group = nullptr;
std::mutex ws_mutex; // Mutex to protect access to the ws_group

// Make the uWS::Hub instance global so it can be accessed by the signal handler
uWS::Hub hub; // Declare hub globally

// --- Function Declarations ---
void signal_handler(int signal);
void file_monitor_thread(const std::string& filepath);

// --- Main Application ---

int main(int argc, char* argv[]) {
    // Check for correct command-line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filepath_to_monitor>\n";
        return 1;
    }
    const std::string filepath = argv[1];
    const int port = 9002;

    // Register signal handlers for graceful shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // --- WebSocket Server Setup ---
    // hub is now a global instance, so no need to declare it here
    ws_group = hub.createGroup<uWS::SERVER>();

    ws_group->onConnection([](uWS::WebSocket<uWS::SERVER>* ws, uWS::HttpRequest req) {
        std::cout << "[Server] Client connected." << std::endl;
    });

    ws_group->onDisconnection([](uWS::WebSocket<uWS::SERVER>* ws, int code, char* message, size_t length) {
        std::cout << "[Server] Client disconnected." << std::endl;
    });

    // We don't need an onMessage handler as this server only broadcasts data
    ws_group->onMessage([](uWS::WebSocket<uWS::SERVER>* ws, char* message, size_t length, uWS::OpCode opCode) {});

    if (!hub.listen(port)) {
        std::cerr << "[Server] Failed to listen on port " << port << std::endl;
        return 1;
    }

    // --- Start File Monitoring in a Background Thread ---
    std::thread monitor_thread(file_monitor_thread, filepath);

    std::cout << "[Server] WebSocket server listening on port " << port << std::endl;
    std::cout << "[Monitor] Watching file: " << filepath << std::endl;

    // --- Run the WebSocket Event Loop (modified for graceful exit) ---
    // Use hub.run(timeout_ms) to allow the loop to periodically check keep_running
    while (keep_running) {
        hub.run(100); // Process events for up to 100ms, then return
    }
    std::cout << "[Main] Hub event loop terminated." << std::endl; // Debug print

    // --- Cleanup ---
    // This part will be reached after the event loop terminates
    std::cout << "[Server] Shutting down..." << std::endl;
    // ensure keep_running is false for monitor thread's final check
    keep_running = false;
    std::cout << "[Main] Attempting to join monitor thread..." << std::endl; // Debug print
    monitor_thread.join(); // Wait for the monitor thread to finish cleanly
    std::cout << "[Main] Monitor thread joined." << std::endl; // Debug print

    return 0;
}

// --- Signal Handler Implementation ---

void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        if (keep_running) {
            std::cerr << "\n[Main] Shutdown signal received. Exiting gracefully...\n";
            keep_running = false; // Signal all loops to stop
            std::cerr << "[Main] Calling hub.stop()..." << std::endl; // Debug print
            hub.stop(); // Explicitly stop the uWS hub's event loop
            std::cerr << "[Main] hub.stop() called." << std::endl; // Debug print
        }
    }
}

// --- File Monitoring Thread Implementation ---

void file_monitor_thread(const std::string& filepath) {
    // Disable buffering for immediate output in this thread
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::cout << std::unitbuf;

    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "[Monitor] Error opening file: " << filepath << std::endl;
        keep_running = false; // Signal main to exit if file cannot be opened
        return;
    }

    // Main loop to read and process the file
    while (keep_running) {
        std::string line;
        if (std::getline(file, line)) {
            if (line.empty()) continue;

            try {
                // Parse the line as a JSON object
                json data = json::parse(line);

                // For HFT, we just forward the raw JSON line.
                // The client GUI will be responsible for parsing and displaying.
                {
                    std::lock_guard<std::mutex> lock(ws_mutex);
                    if (ws_group) {
                        ws_group->broadcast(line.c_str(), line.length(), uWS::OpCode::TEXT);
                    }
                }

            } catch (const json::parse_error& e) {
                // Silently ignore non-JSON lines (e.g., logs, status messages)
            } catch (const std::exception& e) {
                std::cerr << "[Monitor] Data processing error: " << e.what() << std::endl;
            }
        } else {
            if (file.eof()) {
                file.clear(); // Clear EOF flag to allow further reads
            }
            // Sleep briefly to prevent high CPU usage when no new data is available
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    file.close();
    std::cout << "[Monitor] File monitoring thread finished." << std::endl;
}
