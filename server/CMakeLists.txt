cmake_minimum_required(VERSION 3.16)
project(data_monitor_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define paths to both uWebSockets and uSockets
set(UWS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets/uWebSockets)
set(USOCKETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets/uSockets)

# --- Homebrew Cellar Paths ---
# OpenSSL@3 (version 3.5.1)
set(OPENSSL_CELLAR_ROOT /opt/homebrew/Cellar/openssl@3/3.5.1)
set(OPENSSL_INCLUDE_DIR ${OPENSSL_CELLAR_ROOT}/include)
set(OPENSSL_LIBRARIES ${OPENSSL_CELLAR_ROOT}/lib)
include_directories(${OPENSSL_INCLUDE_DIR})
link_directories(${OPENSSL_LIBRARIES})

# libuv (version 1.51.0)
set(LIBUV_CELLAR_ROOT /opt/homebrew/Cellar/libuv/1.51.0)
include_directories(${LIBUV_CELLAR_ROOT}/include)
link_directories(${LIBUV_CELLAR_ROOT}/lib)

# nlohmann/json (version 3.12.0 based on your previous output)
set(NLOHMANN_JSON_CELLAR_ROOT /opt/homebrew/Cellar/nlohmann-json/3.12.0)
set(NLOHMANN_JSON_INCLUDE_DIR ${NLOHMANN_JSON_CELLAR_ROOT}/include)
include_directories(${NLOHMANN_JSON_INCLUDE_DIR})

# Find the OpenSSL library (required for SSL support)
find_package(OpenSSL REQUIRED)

# Collect all .c files from uSockets (core, eventing, crypto)
file(GLOB USOCKETS_SOURCES
    "${USOCKETS_DIR}/src/*.c"
    "${USOCKETS_DIR}/src/eventing/*.c"
    "${USOCKETS_DIR}/src/crypto/*.c"
)

# Build usockets static library
add_library(usockets STATIC ${USOCKETS_SOURCES})

# Enable SSL in uSockets
target_compile_definitions(usockets PRIVATE
    LIBUS_USE_OPENSSL
)

# Suppress OpenSSL deprecation warnings
target_compile_options(usockets PRIVATE
    -Wno-deprecated-declarations
)

# Include usockets headers
target_include_directories(usockets PRIVATE
    "${USOCKETS_DIR}/src"
)

# Link OpenSSL to usockets
target_link_libraries(usockets PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)

# --- Build uWebSockets Library ---
# Collect all .cpp files from uWebSockets
file(GLOB UWS_SOURCES
    "${UWS_DIR}/src/*.cpp"
)

# Build uWebSockets static library
add_library(uwebsockets STATIC ${UWS_SOURCES})

# Include uWebSockets and uSockets headers for uwebsockets library
target_include_directories(uwebsockets PRIVATE
    "${UWS_DIR}/src"
    "${USOCKETS_DIR}/src"
)

# Link uWebSockets to uSockets and other necessary dependencies
target_link_libraries(uwebsockets PRIVATE
    usockets
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
    z
    # Link libuv explicitly if uWebSockets/uSockets needs it directly
    uv # This links against the libuv library found via the Cellar path
)

# Build your main executable
add_executable(new_monitor new_monitor.cpp)

# Include directories for the executable
target_include_directories(new_monitor PRIVATE
    "${UWS_DIR}/src"             # for uWS.h
    "${USOCKETS_DIR}/src"        # for uSockets
    # nlohmann/json is now handled by the include_directories call above
)

# Link the final executable
target_link_libraries(new_monitor PRIVATE
    uwebsockets  # Link against the newly built uwebsockets library
    usockets     # Ensure usockets is also linked directly
    pthread
    z
    uv           # Link libuv explicitly for the main executable as well
)
