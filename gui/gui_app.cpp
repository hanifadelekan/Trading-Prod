#include "gui_app.hpp"
#include "exchanges/hyperliquid/streaming/threads.hpp"
#include "obook.hpp"
#include "bbo.hpp"

#include <imgui.h>
#include <implot.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>
#include <nlohmann/json.hpp>

// Use nlohmann::json for easier JSON manipulation
using json = nlohmann::json;

// ===================================================================================
//
//                                  Constructor & Destructor
//
// ===================================================================================

/**
 * @brief Construct a new GuiApp object.
 *
 * Initializes member variables and calls setup methods.
 */
GuiApp::GuiApp()
    : window_(nullptr),          // Initialize the GLFW window pointer to null
      disruptor_(1024),          // Initialize the disruptor (likely for message passing) with a buffer size of 1024
      obdisruptor_(1024)       // Initialize the order book disruptor with a buffer size of 1024
{
    // Call initialization routines in order
    InitNetwork();
    InitWindow();
    InitImGui();
}

/**
 * @brief Destroy the GuiApp object.
 *
 * Calls the main shutdown routine to clean up all resources.
 */
GuiApp::~GuiApp() {
    Shutdown();
}


// ===================================================================================
//
//                                     Initialization
//
// ===================================================================================

/**
 * @brief Initializes the core networking components (Boost.Asio).
 *
 * Sets up the shared io_context for asynchronous operations and the SSL context
 * for secure connections.
 */
void GuiApp::InitNetwork() {
    // Create a shared io_context to manage all asynchronous I/O operations (e.g., networking)
    shared_io_context_ = std::make_shared<net::io_context>();

    // Create a context for SSL/TLS, specifying TLS 1.2 client protocol
    shared_ssl_context_ = std::make_unique<ssl::context>(ssl::context::tlsv12_client);

    // Set security options for the SSL context to enhance security by disabling outdated protocols
    shared_ssl_context_->set_options(
        ssl::context::default_workarounds | // Use default workarounds for known bugs
        ssl::context::no_sslv2 |            // Disable SSLv2
        ssl::context::no_sslv3 |            // Disable SSLv3
        ssl::context::no_tlsv1 |            // Disable TLSv1.0
        ssl::context::no_tlsv1_1 |          // Disable TLSv1.1
        ssl::context::single_dh_use         // Use a new DH key for each connection
    );

    // Load the default system SSL certificates
    shared_ssl_context_->set_default_verify_paths();

    // Create a "work guard" to keep the io_context running in the background, even if there's no work
    work_guard_ = std::make_unique<work_guard_type>(shared_io_context_->get_executor());
}

/**
 * @brief Initializes the main application window using GLFW.
 *
 * Configures and creates a window suitable for OpenGL rendering.
 */
void GuiApp::InitWindow() {
    // Initialize the GLFW library
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");

    // Set GLFW window hints for the OpenGL context (version 3.2, core profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required for macOS

    // Create the main window
    window_ = glfwCreateWindow(1200, 800, "Hyperliquid Dashboard", nullptr, nullptr);
    if (!window_) {
        glfwTerminate(); // Clean up GLFW if window creation fails
        throw std::runtime_error("Failed to create window");
    }

    // Make the window's OpenGL context current on this thread
    glfwMakeContextCurrent(window_);
    // Enable VSync by setting the swap interval to 1
    glfwSwapInterval(1);
}

/**
 * @brief Initializes the ImGui and ImPlot libraries.
 *
 * Creates contexts, sets styles, and initializes backends for rendering with GLFW and OpenGL.
 */
void GuiApp::InitImGui() {
    // Verify that the compiled ImGui version matches the header version
    IMGUI_CHECKVERSION();
    // Create contexts for ImGui and ImPlot
    ImGui::CreateContext();
    ImPlot::CreateContext();

    // Get the ImGui I/O object for configuration
    ImGuiIO& io = ImGui::GetIO();
    // Enable Docking and Multi-Viewport features
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    io.ConfigViewportsNoAutoMerge = true;
    io.ConfigViewportsNoTaskBarIcon = true;

    // Set the default visual style
    ImGui::StyleColorsDark();

    // Initialize ImGui backends for GLFW and OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 150"); // Specify GLSL version
}

/**
 * @brief Shuts down the application and releases all resources.
 */
void GuiApp::Shutdown() {
    // Release the work guard to allow the io_context to stop
    work_guard_.reset();
    // Explicitly stop the io_context, cancelling any pending async operations
    shared_io_context_->stop();

    // Join all background threads to ensure they finish cleanly
    if (bbo_thread_.joinable()) bbo_thread_.join();
    if (ob_thread_.joinable()) ob_thread_.join();

    // If the WebSocket is connected, close it gracefully
    if (ws_connected_) {
        ws_connected_ = false;
        ws_client_->close(websocket::close_code::normal);
        // Wait for the WebSocket reader thread to exit
        if (ws_thread_.joinable()) ws_thread_.join();
    }

    // Clean up UI components
    if (bbo_viewer_) bbo_viewer_.reset();

    // Shut down ImGui and ImPlot backends and contexts
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    // Destroy the window and terminate GLFW
    if (window_) {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
}


// ===================================================================================
//
//                                Main Application Loop
//
// ===================================================================================

/**
 * @brief The main application loop.
 *
 * Runs continuously until the window is closed. Handles events, renders the GUI,
 * and swaps buffers.
 */
void GuiApp::run() {
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window_)) {
        // Poll for and process events (like keyboard input or window resizing)
        glfwPollEvents();

        // Start a new frame for the ImGui backends and core library
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // Create a dockspace that fills the main viewport
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);

        // Call functions to render individual UI panels
        RenderControlPanel();
        RenderBboSnapshot(0.0);

        // Render all ImGui draw data
        ImGui::Render();

        // Get the framebuffer size for the main window
        int display_w, display_h;
        glfwGetFramebufferSize(window_, &display_w, &display_h);
        // Set the OpenGL viewport to match the window size
        glViewport(0, 0, display_w, display_h);
        // Set the background color
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // Clear the color buffer
        glClear(GL_COLOR_BUFFER_BIT);
        // Render the ImGui draw data using the OpenGL3 backend
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Handle multi-viewport rendering if enabled
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            // Get the current window context to restore it later
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            // Update and render additional platform windows (e.g., dragged-out panels)
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            // Restore the main window's context
            glfwMakeContextCurrent(backup_current_context);
        }

        // Swap the front and back buffers to display the rendered frame
        glfwSwapBuffers(window_);
    }

    // Wait for the reader thread to finish after the loop exits
    if (reader.joinable()) reader.join();
}


// ===================================================================================
//
//                                     UI Rendering
//
// ===================================================================================

/**
 * @brief Renders the main control panel.
 *
 * Contains buttons to start/stop data streams and manage WebSocket connections.
 */
void GuiApp::RenderControlPanel() {
    // Begin a new ImGui window named "Control"
    ImGui::Begin("Control");

    // --- Market Data Controls ---
    ImGui::Text("Control Panel Content Here");

    ImGui::Separator();

    // --- WebSocket Client Controls ---
    ImGui::Text("WS Client");
    // Input fields for the WebSocket host and port
    ImGui::InputText("Host", ws_host_, sizeof(ws_host_));
    ImGui::InputText("Port", ws_port_, sizeof(ws_port_));
    ImGui::SameLine(); // Place the next widget on the same line
    // Button to connect to the WebSocket server
    if (ImGui::Button("Connect WS")) {
        if (!ws_connected_) {
            ConnectToServer(ws_host_, ws_port_);
        }
    }

    ImGui::Separator();

    // --- Coin Selection for Streaming ---
    // A map to hold the state of each coin's checkbox
    static std::map<std::string, bool> coin_checkboxes = {
        {"BTC", false}, {"ETH", false}, {"SOL", false}, {"DOGE", false}, {"XRP", false}
    };
    // A vector to store the names of currently selected coins
    static std::vector<std::string> selected_coins;

    ImGui::Text("Select Coins:");
    // Iterate through the map to create a checkbox for each coin
    for (auto& [coin, is_checked] : coin_checkboxes) {
        bool old_value = is_checked;
        if (ImGui::Checkbox(coin.c_str(), &is_checked)) {
            // If the checkbox state changed, update the selected_coins vector
            if (is_checked && !old_value) {
                selected_coins.push_back(coin); // Add coin if checked
            } else if (!is_checked && old_value) {
                // Remove coin if unchecked
                selected_coins.erase(std::remove(selected_coins.begin(), selected_coins.end(), coin), selected_coins.end());
            }
        }
    }

    ImGui::Separator();
    // Display the list of currently selected coins
    ImGui::Text("Selected Coins:");
    for (const auto& coin : selected_coins) {
        ImGui::BulletText("%s", coin.c_t());
    }
    ImGui::Separator();

    // --- Stream Control Button ---
    // Disable the "Start Stream" button if not connected or no coins are selected
    ImGui::BeginDisabled(!ws_connected_ || selected_coins.empty());
    if (ImGui::Button("Start Stream")) {
        // Create a JSON object with the action and selected coins
        json msg_json = {
            {"action", "start_stream"},
            {"values", selected_coins}
        };
        // Convert the JSON object to a string and send it to the server
        std::string message = msg_json.dump();
        send_message_to_server(message, ws_client_, ws_log_);
    }
    ImGui::EndDisabled(); // End the disabled state

    ImGui::Separator();

    // --- Log Display ---
    // Display the last 5 messages from the WebSocket log
    for (int i = std::max(0, (int)ws_log_.size() - 5); i < ws_log_.size(); ++i) {
        ImGui::Text("%s", ws_log_[i].c_str());
    }

    // End the "Control" window
    ImGui::End();
}


/**
 * @brief Renders the BBO (Best Bid/Offer) snapshot window if enabled.
 *
 * @param dir A parameter to pass to the BBO viewer's render function (purpose unclear from context).
 */
void GuiApp::RenderBboSnapshot(double dir) {
    // Only render if the window is toggled on and the BBO viewer object exists
    if (show_bbo_snapshot_window_ && bbo_viewer_) {
        // Begin a new ImGui window for the BBO snapshot
        ImGui::Begin("BBO Snapshot", &show_bbo_snapshot_window_, ImGuiWindowFlags_NoDocking);
        // Call the BBO viewer's render function to draw the chart/data
        bbo_viewer_->RenderFrame(dir);
        // End the window
        ImGui::End();
    }
}


// ===================================================================================
//
//                                     Networking
//
// ===================================================================================

/**
 * @brief Sends a string message to the connected WebSocket server.
 *
 * @param message The message to send.
 * @param ws_client The WebSocket client stream.
 * @param ws_log A log vector to record sending status.
 */
void send_message_to_server(
    const std::string& message,
    std::unique_ptr<websocket::stream<tcp::socket>>& ws_client,
    std::vector<std::string>& ws_log
) {
    try {
        // Write the message to the WebSocket stream
        ws_client->write(boost::asio::buffer(message));
        // Log success
        ws_log.push_back("[📤] Sent order message");
    } catch (const std::exception& e) {
        // Log any errors that occur during sending
        ws_log.push_back(std::string("[❌] Failed to send: ") + e.what());
    }
}

/**
 * @brief Connects to a WebSocket server and starts a reader thread.
 *
 * @param host The server hostname.
 * @param port The server port.
 */
void GuiApp::ConnectToServer(const std::string& host, const std::string& port) {
    try {
        // Create a resolver to turn the host and port into a list of endpoints
        tcp::resolver resolver{ws_io_};
        auto results = resolver.resolve(host, port);

        // Create the WebSocket stream object
        ws_client_ = std::make_unique<websocket::stream<tcp::socket>>(ws_io_);
        // Connect the underlying TCP socket to the resolved endpoint
        net::connect(ws_client_->next_layer(), results.begin(), results.end());

        // Perform the WebSocket handshake to upgrade the connection
        std::string host_header = host + ":" + port;
        ws_client_->handshake(host_header, "/");

        // Update connection status and log success
        ws_connected_ = true;
        ws_log_.push_back("[+] Connected to " + host + ":" + port);

        // --- Start a new thread to read incoming messages ---
        ws_thread_ = std::thread([this]() {
            try {
                // Loop as long as the connection is active
                while (ws_connected_) {
                    beast::flat_buffer buf; // A buffer to store incoming data
                    ws_client_->read(buf);  // Block until a message is received
                    // Log the received message
                    ws_log_.push_back("[📩] " + beast::buffers_to_string(buf.data()));
                }
            } catch(...) {
                // Log when the connection is lost or an error occurs in the read loop
                ws_log_.push_back("[-] Disconnected");
            }
        });

        // Start the IO context in a detached thread to handle the asynchronous operations
        std::thread([this](){ ws_io_.run(); }).detach();
    } catch (std::exception& e) {
        // Log any exceptions that occur during the connection process
        ws_log_.push_back(std::string("[❌] ") + e.what());
    }
}