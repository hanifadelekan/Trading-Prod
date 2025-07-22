#include <iostream>
#include <fstream>
#include <string>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>
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
std::string global_filepath;

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
        std::cout << "[Server] Client connected. Total clients: " << ++client_count << std::endl;
        if (client_count == 1) {
            // First client - start file thread
            keep_file_running = true;
            file_thread = std::thread(file_monitor_thread);
            std::cout << "[Monitor] Starting file monitoring." << std::endl;
        }
    });

    ws_group->onDisconnection([](uWS::WebSocket<uWS::SERVER>* ws, int code, char* message, size_t length) {
        std::cout << "[Server] Client disconnected. Total clients: " << --client_count << std::endl;
        if (client_count == 0) {
            // Last client - stop file thread
            keep_file_running = false;
            if (file_thread.joinable()) {
                file_thread.join();
                std::cout << "[Monitor] File monitoring stopped." << std::endl;
            }
        }
    });

    ws_group->onMessage([](uWS::WebSocket<uWS::SERVER>* ws, char* message, size_t length, uWS::OpCode opCode) {});

    // Fixed error handler
    ws_group->onError([](int code) {
        std::cerr << "[Server] WebSocket error occurred. Code: " << code << std::endl;
    });

    if (!hub.listen(port)) {
        std::cerr << "[Server] Failed to listen on port " << port << std::endl;
        return 1;
    }

    std::cout << "[Server] WebSocket server listening on port " << port << std::endl;
    std::cout << "[Monitor] File to watch: " << global_filepath << std::endl;

    hub.run();

    // Server shutdown sequence
    keep_running = false;
    keep_file_running = false;

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
            // Close all connections and stop the hub
            ws_group->close();
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

    // Start reading from the end of the file
    file.seekg(0, std::ios::end);
    std::streampos last_pos = file.tellg();
    int line_count = 0;
    int broadcast_count = 0;
    auto last_log_time = std::chrono::steady_clock::now();

    std::cout << "[Monitor] Starting monitoring from end of file\n";

    while (keep_file_running && keep_running) {
        std::string line;
        
        // Check if file was rotated (common with hourly files)
        std::ifstream new_check(global_filepath);
        if (!new_check) {
            std::cerr << "[Monitor] File missing, waiting..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        
        new_check.seekg(0, std::ios::end);
        if (new_check.tellg() < last_pos) {
            std::cout << "[Monitor] File reset detected, reopening..." << std::endl;
            file.close();
            file.open(global_filepath);
            if (!file) {
                std::cerr << "[Monitor] Reopen failed" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
            last_pos = 0;
        }
        
        // Read new lines
        file.clear(); // Clear any error flags
        file.seekg(last_pos);
        while (std::getline(file, line)) {
            if (!line.empty()) {
                try {
                    // Validate JSON before broadcasting
                    json data = json::parse(line);
                    {
                        std::lock_guard<std::mutex> lock(ws_mutex);
                        if (ws_group) {
                            ws_group->broadcast(line.c_str(), line.length(), uWS::OpCode::TEXT);
                            broadcast_count++;
                        }
                    }
                } catch (const json::parse_error&) {
                    // Silently ignore non-JSON lines
                }
                line_count++;
            }
        }
        
        // Update position and handle EOF
        last_pos = file.tellg();
        if (file.eof()) {
            file.clear();
        }
        
        // Periodic logging
        auto now = std::chrono::steady_clock::now();
        if (now - last_log_time > std::chrono::seconds(5)) {
            std::cout << "[Monitor] Status: " << line_count << " lines processed, "
                      << broadcast_count << " broadcasts" << std::endl;
            last_log_time = now;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    file.close();
    std::cout << "[Monitor] Exiting after " << line_count << " lines processed\n";
}