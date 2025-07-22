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

using json = nlohmann::json;

// --- Global Variables ---
std::atomic<bool> keep_running(true);
uWS::Group<uWS::SERVER>* ws_group = nullptr;
std::mutex ws_mutex;
uWS::Hub hub;

// File thread management
std::thread file_thread;
std::atomic<int> client_count(0);
std::atomic<bool> keep_file_running(false);
std::string global_filepath;  // Stores the file path to monitor

// --- Function Declarations ---
void signal_handler(int signal);
void file_monitor_thread();

// --- Main Application ---

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filepath_to_monitor>\n";
        return 1;
    }
    global_filepath = argv[1];
    const int port = 9002;

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    ws_group = hub.createGroup<uWS::SERVER>();

    ws_group->onConnection([](uWS::WebSocket<uWS::SERVER>* ws, uWS::HttpRequest req) {
        std::cout << "[Server] Client connected." << std::endl;
        if (client_count++ == 0) {
            // First client - start file thread
            keep_file_running = true;
            file_thread = std::thread(file_monitor_thread);
            std::cout << "[Monitor] Starting file monitoring." << std::endl;
        }
    });

    ws_group->onDisconnection([](uWS::WebSocket<uWS::SERVER>* ws, int code, char* message, size_t length) {
        std::cout << "[Server] Client disconnected." << std::endl;
        if (--client_count == 0) {
            // Last client - stop file thread
            keep_file_running = false;
            if (file_thread.joinable()) {
                file_thread.join();
                std::cout << "[Monitor] File monitoring stopped." << std::endl;
            }
        }
    });

    ws_group->onMessage([](uWS::WebSocket<uWS::SERVER>* ws, char* message, size_t length, uWS::OpCode opCode) {});

    if (!hub.listen(port)) {
        std::cerr << "[Server] Failed to listen on port " << port << std::endl;
        return 1;
    }

    std::cout << "[Server] WebSocket server listening on port " << port << std::endl;
    std::cout << "[Monitor] File to watch: " << global_filepath << std::endl;

    hub.run();

    // Server shutdown sequence
    keep_running = false;
    keep_file_running = false;  // Ensure file thread stops

    if (file_thread.joinable()) {
        file_thread.join();
    }

    return 0;
}

// --- Signal Handler Implementation ---

void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        if (keep_running) {
            std::cerr << "\n[Main] Shutdown signal received. Exiting gracefully...\n";
            keep_running = false;
            hub.uWS::Hub::getDefaultGroup<uWS::SERVER>().close();
        }
    }
}

// --- File Monitoring Thread Implementation ---

void file_monitor_thread() {
    std::ifstream file(global_filepath);
    if (!file) {
        std::cerr << "[Monitor] Error opening file: " << global_filepath << std::endl;
        return;
    }

    // Main processing loop
    while (keep_file_running && keep_running) {
        std::string line;
        if (std::getline(file, line)) {
            if (line.empty()) continue;

            try {
                // Validate JSON before broadcasting
                json data = json::parse(line);
                {
                    std::lock_guard<std::mutex> lock(ws_mutex);
                    if (ws_group) {
                        ws_group->broadcast(line.c_str(), line.length(), uWS::OpCode::TEXT);
                    }
                }
            } catch (const json::parse_error&) {
                // Silently ignore non-JSON lines
            }
        } else {
            if (file.eof()) {
                file.clear();  // Clear EOF flag
                file.seekg(0, std::ios::end);  // Seek to end for new data
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    file.close();
}