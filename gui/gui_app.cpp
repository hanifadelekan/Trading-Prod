#include "gui_app.hpp"
#include <imgui.h>
#include <implot.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>
#include "exchanges/hyperliquid/streaming/threads.hpp"
#include "obook.hpp"
#include "bbo.hpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
GuiApp::GuiApp()
    : window_(nullptr),
      disruptor_(1024),
      obdisruptor_(1024)
      //order_manager_(disruptor_)
{
    InitNetwork();
    InitWindow();
    InitImGui();
    //order_manager_.init();
}

GuiApp::~GuiApp() {
    Shutdown();
}

void GuiApp::InitNetwork() {
    shared_io_context_ = std::make_shared<net::io_context>();
    shared_ssl_context_ = std::make_unique<ssl::context>(ssl::context::tlsv12_client);
    shared_ssl_context_->set_options(
        ssl::context::default_workarounds |
        ssl::context::no_sslv2 | ssl::context::no_sslv3 |
        ssl::context::no_tlsv1 | ssl::context::no_tlsv1_1 |
        ssl::context::single_dh_use
    );
    shared_ssl_context_->set_default_verify_paths();
    work_guard_ = std::make_unique<work_guard_type>(shared_io_context_->get_executor());
}

void GuiApp::InitWindow() {
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window_ = glfwCreateWindow(1200, 800, "Hyperliquid Dashboard", nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
}

void GuiApp::InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    io.ConfigViewportsNoAutoMerge = true;
    io.ConfigViewportsNoTaskBarIcon = true;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 150");
}

void GuiApp::Shutdown() {
    work_guard_.reset();
    shared_io_context_->stop();
    if (bbo_thread_.joinable()) bbo_thread_.join();
    if (ob_thread_.joinable()) ob_thread_.join();
    if (ws_connected_) {
        ws_connected_ = false;
        ws_client_->close(websocket::close_code::normal);
        if (ws_thread_.joinable()) ws_thread_.join();
    }

    if (bbo_viewer_) bbo_viewer_.reset();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    if (window_) {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
}

void send_message_to_server(
    const std::string& message,
    std::unique_ptr<websocket::stream<tcp::socket>>& ws_client,
    std::vector<std::string>& ws_log  // <-- fix here!
) {
    try {
        ws_client->write(boost::asio::buffer(message));
        ws_log.push_back("[📤] Sent order message");
    } catch (const std::exception& e) {
        ws_log.push_back(std::string("[❌] Failed to send: ") + e.what());
    }
}



void GuiApp::run() {
    
    while (!glfwWindowShouldClose(window_)) {
        
        
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);

        RenderControlPanel();
        RenderBboSnapshot(0.0);


        //order_manager_.process();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window_, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window_);
    }
    if (reader.joinable()) reader.join();
}

void GuiApp::RenderControlPanel() {
    if (!control_window_initial_size_set_) {
        control_window_initial_size_set_ = true;
    }

    ImGui::Begin("Control");
    ImGui::Text("Control Panel Content Here");
    if (!hl_market_data_started_) {
        if (ImGui::Button("Start HL Market Data")) {
            StartMarketDataStreams(this,"hyperliquid");
        }
    } else {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Market Data Running");
        if (ImGui::Button("Toggle Market Data Chart")) {
            show_bbo_snapshot_window_ = !show_bbo_snapshot_window_;
        }
    }
    //if (!hl_trading_started) {
        //if (ImGui::Button("Start HL Trading")) {
            //StartTrading("hyperliquid");
       // }
    //} else {
       // ImGui::TextColored(ImVec4(0, 1, 0, 1), "Trading Running");
   // }
    // ——— WebSocket client controls ———
    ImGui::Separator();
    ImGui::Text("WS Client");
    ImGui::InputText("Host", ws_host_, sizeof(ws_host_));
    ImGui::InputText("Port", ws_port_, sizeof(ws_port_));
    ImGui::SameLine();
    if (ImGui::Button("Connect WS")) {
        if (!ws_connected_) {
            ConnectToServer(ws_host_, ws_port_);
        }
    }

    // Log last 5 messages
    for (int i = std::max(0, (int)ws_log_.size() - 5); i < ws_log_.size(); ++i) {
        ImGui::Text("%s", ws_log_[i].c_str());
    }
    static std::map<std::string, bool> coin_checkboxes = {
    {"BTC", false},
    {"ETH", false},
    {"SOL", false},
    {"DOGE", false},
    {"XRP", false}
};

static std::vector<std::string> selected_coins;

ImGui::Text("Select Coins:");
for (auto& [coin, is_checked] : coin_checkboxes) {
    bool old_value = is_checked;
    if (ImGui::Checkbox(coin.c_str(), &is_checked)) {
        if (is_checked && !old_value) {
            selected_coins.push_back(coin);
        } else if (!is_checked && old_value) {
            selected_coins.erase(std::remove(selected_coins.begin(), selected_coins.end(), coin), selected_coins.end());
        }
    }
}

ImGui::Separator();
ImGui::Text("Selected Coins:");
for (const auto& coin : selected_coins) {
    ImGui::BulletText("%s", coin.c_str());
}

ImGui::Separator();

// Only enable button if WebSocket is connected and at least one coin is selected
ImGui::BeginDisabled(!ws_connected_ || selected_coins.empty());
if (ImGui::Button("Start Stream")) {
    // Build JSON-like message manually or with a library
    

json msg_json = {
    {"action", "start_stream"},
    {"values", selected_coins}
};

std::string message = msg_json.dump();  // Convert to JSON string
send_message_to_server(message, ws_client_, ws_log_);

}
ImGui::EndDisabled();



    ImGui::End();
}

void GuiApp::RenderBboSnapshot(double dir) {
    if (show_bbo_snapshot_window_ && bbo_viewer_) {
        ImGui::Begin("BBO Snapshot", &show_bbo_snapshot_window_, ImGuiWindowFlags_NoDocking);
        bbo_viewer_->RenderFrame(dir);
        
        ImGui::End();
    }
}


void GuiApp::ConnectToServer(const std::string& host, const std::string& port) {
    try {
        // Create client and resolver
        tcp::resolver resolver{ws_io_};
        auto results = resolver.resolve(host, port);

        ws_client_ = std::make_unique<websocket::stream<tcp::socket>>(ws_io_);
        net::connect(ws_client_->next_layer(), results.begin(), results.end());

        std::string host_header = host + ":" + port;
        ws_client_->handshake(host_header, "/");

        ws_connected_ = true;
        ws_log_.push_back("[+] Connected to " + host + ":" + port);

        // start reader thread
        ws_thread_ = std::thread([this]() {
            try {
                while (ws_connected_) {
                    beast::flat_buffer buf;
                    ws_client_->read(buf);
                    ws_log_.push_back("[📩] " +
                        beast::buffers_to_string(buf.data()));
                }
            } catch(...) {
                ws_log_.push_back("[-] Disconnected");
            }
        });

        // Run the IO context in background
        std::thread([this](){ ws_io_.run(); }).detach();
    } catch (std::exception& e) {
        ws_log_.push_back(std::string("[❌] ") + e.what());
    }
}

