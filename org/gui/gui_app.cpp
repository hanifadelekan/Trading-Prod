#include "gui_app.hpp"
#include <imgui.h>
#include <implot.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>
#include "exchanges/hyperliquid/streaming/threads.hpp"
#include "obook.hpp"
#include "bbo.hpp"

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
    ImGui::End();
}

void GuiApp::RenderBboSnapshot(double dir) {
    if (show_bbo_snapshot_window_ && bbo_viewer_) {
        ImGui::Begin("BBO Snapshot", &show_bbo_snapshot_window_, ImGuiWindowFlags_NoDocking);
        bbo_viewer_->RenderFrame(dir);
        ImGui::End();
    }
}




