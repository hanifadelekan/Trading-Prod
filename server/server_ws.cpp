#include <uWS/uWS.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <algorithm>
#include <condition_variable>
#include <chrono>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct PerSocketData {
    // Per-socket data structure
};

std::vector<pid_t> child_pids;
std::mutex pids_mutex;
std::atomic<int> active_clients{0};
std::mutex timer_mutex;
std::condition_variable timer_cv;

void run_command_in_background(const std::string& cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        setsid();
        prctl(PR_SET_PDEATHSIG, SIGTERM);
        execlp("/bin/sh", "sh", "-c", cmd.c_str(), (char*)nullptr);
        perror("execlp");
        exit(1);
    } else if (pid > 0) {
        std::lock_guard<std::mutex> lock(pids_mutex);
        child_pids.push_back(pid);
        
        std::thread([pid, cmd]() {
            int status;
            waitpid(-pid, &status, 0);
            
            {
                std::lock_guard<std::mutex> lock(pids_mutex);
                auto it = std::find(child_pids.begin(), child_pids.end(), pid);
                if (it != child_pids.end()) {
                    child_pids.erase(it);
                }
            }
            std::cout << "[*] Process group " << pid << " finished." << std::endl;
        }).detach();
    } else {
        perror("fork");
    }
}

void terminate_child_processes() {
    std::lock_guard<std::mutex> lock(pids_mutex);
    for (pid_t pid : child_pids) {
        kill(-pid, SIGTERM);
        std::cout << "[*] Sent SIGTERM to process group " << pid << std::endl;
    }
    child_pids.clear();
}

void signal_handler(int signum) {
    std::cout << "\n[*] Caught signal " << signum << ". Terminating child processes." << std::endl;
    terminate_child_processes();
    exit(signum);
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    int port = 9002;

    uWS::Hub hub;
    
    // Configure WebSocket behavior
    uWS::Group<uWS::SERVER>* group = hub.createGroup<uWS::SERVER>();
    group->onConnection([](uWS::WebSocket<uWS::SERVER>* ws, uWS::HttpRequest req) {
        std::cout << "[+] Client connected." << std::endl;
        active_clients++;
        timer_cv.notify_all();
    });
    
    group->onMessage([](uWS::WebSocket<uWS::SERVER>* ws, char* message, size_t length, uWS::OpCode opCode) {
        std::string_view msg_view(message, length);
        try {
            auto data = json::parse(msg_view);
            std::cout << "[📩] Received: " << msg_view << std::endl;
            
            if (data.contains("action")) {
                if (data["action"] == "start_stream") {
                    std::cout << "[*] Starting monitoring stream" << std::endl;
                    
                    const std::string monitor_cmd = "./stripped_monitor";
                    const std::string visor_cmd = "~/hl-visor run-non-validator --write-raw-book-diffs --disable-output-file-buffering";
                    
                    run_command_in_background(monitor_cmd);
                    run_command_in_background(visor_cmd);

                    ws->send("OK: Monitoring started", opCode);
                }
                else if (data["action"] == "stop_stream") {
                    std::cout << "[*] Stopping monitoring stream" << std::endl;
                    terminate_child_processes();
                    ws->send("OK: Monitoring stopped", opCode);
                }
                else {
                    ws->send("ERROR: Unknown action", opCode);
                }
            }
        } catch (json::parse_error& e) {
            std::cerr << "[!] JSON parse error: " << e.what() << std::endl;
            ws->send("ERROR: Invalid JSON", opCode);
        }
    });
    
    group->onDisconnection([](uWS::WebSocket<uWS::SERVER>* ws, int code, char* message, size_t length) {
        std::cout << "[-] Client disconnected" << std::endl;
        if (--active_clients == 0) {
            std::thread([] {
                std::unique_lock<std::mutex> lock(timer_mutex);
                if (timer_cv.wait_for(lock, std::chrono::minutes(5)) == std::cv_status::timeout) {
                    std::cout << "[*] No clients for 5 minutes. Stopping processes" << std::endl;
                    terminate_child_processes();
                }
            }).detach();
        }
    });
    
    // Start listening
    if (!hub.listen(port)) {
        std::cerr << "[!] Failed to listen on port " << port << std::endl;
        return 1;
    }
    
    std::cout << "✅ Server listening on port " << port << std::endl;
    std::cout << "[*] Send {\"action\":\"start_stream\"} to begin" << std::endl;
    
    // Run the event loop
    hub.run();

    return 0;
}
