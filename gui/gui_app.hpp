#pragma once

#include <memory>
#include <thread>

// existing includes…
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/websocket.hpp>    // ← add
#include <boost/beast/core.hpp>         // ← add
#include <boost/asio/connect.hpp>       // ← add
#include <boost/beast/http.hpp>         // may be pulled in by core
#include "disruptor.h"
#include "orderbook.hpp"
#include "imgui_bbo_viewer.hpp"
#include "obook.hpp"
#include "bbo.hpp"
#include "order_manager.hpp"

namespace ssl = boost::asio::ssl;
namespace net = boost::asio;
namespace beast     = boost::beast;        // ← add
namespace websocket = beast::websocket;     // ← add
using tcp           = net::ip::tcp;        // ← add

class GuiApp {
public:
    GuiApp();
    ~GuiApp();

    void run();
    void MainLoop();

    bool hl_market_data_started_ = false;
    std::thread bbo_thread_;

    // Make these public to allow external setup in threads.cpp
    using work_guard_type = net::executor_work_guard<net::io_context::executor_type>;
    std::shared_ptr<net::io_context> shared_io_context_;
    std::unique_ptr<ssl::context> shared_ssl_context_;
    std::unique_ptr<work_guard_type> work_guard_;

    std::unique_ptr<ImGuiBBOViewer> bbo_viewer_;
    GLFWwindow* window_;
    Disruptor<BBOSnapshot> disruptor_;
    Disruptor<OBSnapshot> obdisruptor_;
    OrderBook order_book_;

    bool show_bbo_snapshot_window_ = false;

private:
    void InitNetwork();
    void InitWindow();
    void InitImGui();
    void Shutdown();

    void RenderControlPanel();
    void RenderBboSnapshot(double);

    double fair_price_(double level_1_imbalance, double imbalance, double midprice, double midprice_return);
    double calc_return(double latest_midprice);
    
    enum ImbalanceCode { LOW, EQUAL, HIGH };
    ImbalanceCode get_imbalance_code(double value);

    void sendorders(std::string side, std::string orderid, double price, double size);
    void portfolio_manager(double fair_price, double midprice, double current_position, double phantom_position);

    // ——— New WebSocket client members ———
    net::io_context                           ws_io_;
    std::unique_ptr<websocket::stream<tcp::socket>> ws_client_;
    std::thread                               ws_thread_;
    bool                                      ws_connected_ = false;
    char                                      ws_host_[64] = "13.230.206.173";
    char                                      ws_port_[8]  = "9002";
    std::vector<std::string>                  ws_log_;

    // Helper to connect on button press:
    void ConnectToServer(const std::string& host, const std::string& port);

    std::thread reader;
    bool control_window_initial_size_set_ = false;
    bool hl_trading_started_ = false;
    std::thread ob_thread_;
    double current_midprice_ = 0.0;
};
