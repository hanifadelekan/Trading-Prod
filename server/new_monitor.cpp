#include <iostream>
#include <fstream>
#include <string>
#include <csignal>
#include <atomic>

std::atomic<bool> keep_running(true);

void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        keep_running = false;
        std::cerr << "\n[Monitor] Exiting...\n";
    }
}

int main(int argc, char* argv[]) {
    std::setvbuf(stdout, nullptr, _IONBF, 0);  // Fully disable buffering
    std::cout << std::unitbuf;                // Flush after every output

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filepath>\n";
        return 1;
    }

    const std::string filepath = argv[1];
    std::streampos last_pos = 0;

    // Handle Ctrl+C
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "[Monitor] Error opening file: " << filepath << std::endl;
        return 1;
    }

    std::cout << "[Monitor] Watching: " << filepath << std::endl;

    // Initially seek to beginning or end as needed (start reading from start here)
    file.seekg(last_pos);

    bool printed_header = false;

    while (keep_running) {
        std::string line;

        // Try reading a line
        if (std::getline(file, line)) {
            if (!printed_header) {
                std::cout << "\n--- New content in " << filepath << " ---\n";
                printed_header = true;
            }
            std::cout << line << std::endl;
            last_pos = file.tellg();
        } else {
            // No new line available
            printed_header = false;
            if (file.eof()) {
                file.clear();  // Clear EOF flag so next read attempts succeed
            } else {
                // Some other error - possibly reopen file if desired
                std::cerr << "[Monitor] Error or file truncated? Reopening file...\n";
                file.close();
                file.open(filepath);
                if (!file) {
                    std::cerr << "[Monitor] Failed to reopen file: " << filepath << std::endl;
                    break;
                }
                // Reset last_pos to 0 to start over
                last_pos = 0;
                file.seekg(last_pos);
            }
        }

        // No sleep here, as per your request; this loop will spin until new data arrives
    }

    file.close();
    return 0;
}

