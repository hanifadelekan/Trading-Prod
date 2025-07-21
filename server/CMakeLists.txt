cmake_minimum_required(VERSION 3.16)
project(ws_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ✅ Build uSockets library with SSL support
file(GLOB USOCKETS_SOURCES
    /home/hyperliquid_user/uWebSockets/uSockets/src/*.c
    /home/hyperliquid_user/uWebSockets/uSockets/src/eventing/*.c
    /home/hyperliquid_user/uWebSockets/uSockets/src/crypto/*.c
    # Add our custom SNI implementation
    /home/hyperliquid_user/uWebSockets/uSockets/src/crypto/sni.c
)

add_library(usockets STATIC ${USOCKETS_SOURCES})

# ✅ Enable SSL support in uSockets
target_compile_definitions(usockets PRIVATE
    LIBUS_USE_OPENSSL
)

# ✅ Suppress OpenSSL deprecation warnings
target_compile_options(usockets PRIVATE
    -Wno-deprecated-declarations
)

target_include_directories(usockets PRIVATE
    /home/hyperliquid_user/uWebSockets/uSockets/src
)
link_directories(/usr/lib /usr/lib64)

# ✅ Main executable
add_executable(ws_server server_ws.cpp)

# ✅ Include directories
target_include_directories(ws_server PRIVATE 
    /usr/include/uWS 
    /home/hyperliquid_user/uWebSockets/src
    /home/hyperliquid_user/uWebSockets/uSockets/src
)

# ✅ Link against uSockets and required system libraries
target_link_libraries(ws_server PRIVATE 
    usockets
    uWS
    pthread 
    ssl 
    crypto 
    z
)
