#include "gui_app.hpp"
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
//                              Constructor & Destructor
//
// ===================================================================================

/**
 * @brief Construct a new GuiApp object.
 */
GuiApp::GuiApp()
    : window_(nullptr)
{
    // Initialize the BBO viewer UI component
    bbo_viewer_ = std::make_unique<ImGuiBBOViewer>();

    // Initialize the WebSocket receiver and give it a reference to the viewer
    ws_receiver_ = std::make_unique<WebSocketReceiver>(*bbo_viewer_);

    // Initialize the window and ImGui
    InitWindow();
    InitImGui();
}

/**
 * @brief Destroy the GuiApp object.
 */
GuiApp::~GuiApp() {
    Shutdown();
}

// ===================================================================================
//
//                                     Initialization & Shutdown
//
// ===================================================================================

/**
 * @brief Initializes the main application window using GLFW.
 */
void GuiApp::InitWindow() {
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window_ = glfwCreateWindow(1200, 800, "Trading Dashboard", nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
}

/**
 * @brief Initializes the ImGui and ImPlot libraries.
 */
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

/**
 * @brief Shuts down the application and releases all resources.
 */
void GuiApp::Shutdown() {
    // Disconnect the WebSocket receiver, which will join its background thread
    if (ws_receiver_) {
        ws_receiver_->Disconnect();
    }

    // Clean up UI components
    bbo_viewer_.reset();

    // Shut down ImGui and ImPlot
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
 */
void GuiApp::run() {
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);

        RenderControlPanel();
        RenderBboSnapshot(0.0); // The 'dir' parameter can be updated from your logic

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
}

// ===================================================================================
//
//                                     UI Rendering
//
// ===================================================================================

/**
 * @brief Renders the main control panel.
 */
void GuiApp::RenderControlPanel() {
    ImGui::Begin("Control");

    ImGui::Text("WebSocket Controls");
    ImGui::InputText("Host", ws_host_, sizeof(ws_host_));
    ImGui::InputText("Port", ws_port_, sizeof(ws_port_));

    if (ws_receiver_ && !ws_receiver_->IsConnected()) {
        if (ImGui::Button("Connect")) {
            ws_receiver_->Connect(ws_host_, ws_port_);
        }
    } else {
        if (ImGui::Button("Disconnect")) {
            ws_receiver_->Disconnect();
        }
    }
    
    ImGui::Separator();
    ImGui::Text("Chart Controls");
    ImGui::Checkbox("Show BBO Chart", &show_bbo_snapshot_window_);

    ImGui::End();
}

/**
 * @brief Renders the BBO snapshot window if enabled.
 */
void GuiApp::RenderBboSnapshot(double dir) {
    if (show_bbo_snapshot_window_ && bbo_viewer_) {
        ImGui::Begin("BBO Snapshot", &show_bbo_snapshot_window_);
        bbo_viewer_->RenderFrame(dir);
        ImGui::End();
    }
}
