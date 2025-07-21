#ifndef GUI_APP_HPP
#define GUI_APP_HPP

#include <memory>
#include "imgui_bbo_viewer.hpp"
#include "websocket_receivers.hpp"

// Forward-declare GLFWwindow to avoid including GLFW headers here
struct GLFWwindow;

class GuiApp {
public:
    GuiApp();
    ~GuiApp();

    void run();

private:
    // --- Initialization & Shutdown ---
    void InitWindow();
    void InitImGui();
    void Shutdown();

    // --- UI Rendering ---
    void RenderControlPanel();
    void RenderBboSnapshot(double dir);

    // --- Member Variables ---
    GLFWwindow* window_;

    // UI Components
    std::unique_ptr<ImGuiBBOViewer> bbo_viewer_;

    // Networking Component
    std::unique_ptr<WebSocketReceiver> ws_receiver_;

    // UI State
    char ws_host_[128] = "127.0.0.1";
    char ws_port_[16] = "8080";
    bool show_bbo_snapshot_window_ = false;
};

#endif // GUI_APP_HPP
