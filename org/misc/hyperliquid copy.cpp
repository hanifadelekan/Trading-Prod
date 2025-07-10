#include "streaming_main.hpp"
#include "ws_reader.hpp"
#include "resolve_ws.hpp"



auto [ws, buffer] = start_websocket_stream("api.hyperliquid.xyz", "443", "/ws",io_context, ssl_ctx);

    if (!ws || !buffer) return; 

    hyperliquid_subscribe(ws, symbol, ec);
    start_async_read(ws, buffer, io_context, ...);
