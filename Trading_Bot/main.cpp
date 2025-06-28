#include "fetch_hyperliquid_bbo.h"
#include "fetch_hyperliquid_orderbook.h"
#include "imgui_bbo_viewer.h"
#include "order_manager.hpp"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <thread>
#include <iostream>
#include <implot.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <memory>
#include "fetch_hyperliquid_bbo.h"
#include "disruptor.h"
#include "hyperliquid_parser.h"
#include "order_manager.hpp"

// Global disruptor now uses BBOSnapshot, not vector<BBOLevel>
Disruptor<BBOSnapshot> disruptor(1024);

HyperliquidParser parser;

namespace net = boost::asio;
namespace ssl = net::ssl;

std::thread bbo_thread;
std::thread ob_thread;

std::shared_ptr<net::io_context> shared_io_context;
std::unique_ptr<ssl::context> shared_ssl_context;
using work_guard_type = net::executor_work_guard<net::io_context::executor_type>;
std::unique_ptr<work_guard_type> work_guard;

int main() {
    shared_io_context = std::make_shared<net::io_context>();
    shared_ssl_context = std::make_unique<ssl::context>(ssl::context::tlsv12_client);
    shared_ssl_context->set_options(ssl::context::default_workarounds | ssl::context::no_sslv2 | ssl::context::no_sslv3 | ssl::context::no_tlsv1 | ssl::context::no_tlsv1_1 | ssl::context::single_dh_use);
    shared_ssl_context->set_default_verify_paths();
    work_guard = std::make_unique<work_guard_type>(shared_io_context->get_executor());

    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1200, 800, "Hyperliquid Dashboard", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION(); ImGui::CreateContext(); ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    io.ConfigViewportsNoAutoMerge = true;
    io.ConfigViewportsNoTaskBarIcon = true;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    bbo_thread = std::thread([]() {
        std::cout << "BBO thread started. Setting up stream...\n";
        run_bbo_async_stream(shared_io_context, *shared_ssl_context, "BTC", "bbo");
        shared_io_context->run();
        std::cout << "BBO thread io_context finished.\n";
    });

    ob_thread = std::thread([]() {
        std::cout << "Order Book thread started. Setting up stream...\n";
        run_orderbook_async_stream(shared_io_context, *shared_ssl_context, "BTC");
        shared_io_context->run();
        std::cout << "Order Book thread io_context finished.\n";
    });

    ImGuiBBOViewer viewer(window);
    OrderManager order_manager;
    order_manager.init();

    // A flag to indicate if we've set an initial size for the control window
    bool control_window_initial_size_set = false;


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);

        // Draw BBO Snapshot window, resizable & movable by user
        ImGui::Begin("BBO Snapshot");
        viewer.RenderFrame();
        ImGui::End();

        // --- Draw Control panel ---
        // Conditionally set initial size if not already set
        if (!control_window_initial_size_set) {
             ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Once); // Set a reasonable initial size
             control_window_initial_size_set = true;
        }

        // Removed ImGuiWindowFlags_AlwaysAutoResize to allow user resizing
        ImGui::Begin("Control", nullptr, ImGuiWindowFlags_NoDocking);
        ImGui::Text("Control Panel Content Here");
        // Add your control panel elements here
        if (ImGui::Button("Button 1")) {
            std::cout << "Button 1 clicked!\n";
        }
        // Removed "Fullscreen Control Window" checkbox
        // Removed "Target Monitor" dropdown

        ImGui::End(); // End "Control" window

        // --- No more logic for fullscreen or target monitor in this block ---
        // The window will behave as a standard OS window with its own title bar
        // and be user-resizable. If you want to remove the decoration and
        // the user has to drag it manually, that's possible too, but removing
        // the decoration usually comes with the expectation of programmatic control
        // over position/size (like a fullscreen toggle).
        // Since you removed the programmatic control, leaving decorations means
        // the user has the usual OS window controls.

        order_manager.process();
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    std::cout << "Shutting down...\n";
    work_guard.reset();
    shared_io_context->stop(); // Signal io_context to stop

    if (bbo_thread.joinable()) bbo_thread.join();
    if (ob_thread.joinable()) ob_thread.join();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}