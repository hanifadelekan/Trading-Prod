#ifndef WEBSOCKET_RECEIVERS_HPP
#define WEBSOCKET_RECEIVERS_HPP

#include <string>
#include <memory>
#include <thread>
#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include "imgui_bbo_viewer.hpp" // The viewer to push data to

namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace websocket = boost::beast::websocket;

class WebSocketReceiver {
public:
    // --- Constructor ---
    // Takes a reference to the viewer it will update.
    WebSocketReceiver(ImGuiBBOViewer& viewer);
    ~WebSocketReceiver();

    // --- Public Control Methods ---
    void Connect(const std::string& host, const std::string& port);
    void Disconnect();
    bool IsConnected() const;

private:
    // --- Internal Methods ---
    void Listen(); // The main loop for reading messages
    void ParseMessage(const std::string& message); // Logic to parse JSON

    // --- Member Variables ---
    ImGuiBBOViewer& viewer_; // Reference to the UI component

    net::io_context ioc_; // Asio's core I/O object
    std::unique_ptr<websocket::stream<tcp::socket>> ws_stream_; // The WebSocket stream
    std::thread listener_thread_; // The dedicated thread for listening to messages

    std::atomic<bool> is_connected_{false}; // Flag for connection status
};

#endif // WEBSOCKET_RECEIVERS_HPP
