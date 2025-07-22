cmake_minimum_required(VERSION 3.16)
project(data_monitor_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define paths to both uWebSockets and uSockets
set(UWS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets/uWebSockets)
set(USOCKETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets/uSockets)

# --- Find System Libraries (for Ubuntu/Standard Linux) ---
# OpenSSL
# CMake's find_package will locate OpenSSL in standard system paths (e.g., /usr/lib, /usr/include)
find_package(OpenSSL REQUIRED)

# libuv
# Use PkgConfig to find libuv, which is common on Linux distributions
find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(LIBUV REQUIRED libuv)
    if (NOT LIBUV_FOUND)
        message(FATAL_ERROR "libuv not found via PkgConfig. Please ensure libuv-dev is installed and pkg-config is set up correctly.")
    endif()
else()
    # Fallback if PkgConfig is not found or not preferred
    message(STATUS "PkgConfig not found, attempting manual libuv discovery.")
    find_path(LIBUV_INCLUDE_DIR NAMES uv.h PATHS /usr/include /usr/local/include /usr/include/libuv)
    find_library(LIBUV_LIBRARY NAMES uv PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)
    if(LIBUV_INCLUDE_DIR AND LIBUV_LIBRARY)
        message(STATUS "Found libuv manually: ${LIBUV_LIBRARY}")
        set(LIBUV_INCLUDE_DIRS ${LIBUV_INCLUDE_DIR})
        set(LIBUV_LIBRARIES ${LIBUV_LIBRARY})
    else()
        message(FATAL_ERROR "libuv not found manually. Please install libuv-dev package.")
    endif()
endif()

# nlohmann/json (typically header-only, but can be found via find_package if installed via apt)
find_package(nlohmann_json CONFIG REQUIRED) # Assumes nlohmann-json-dev package provides a CMake config

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
    ${LIBUV_INCLUDE_DIRS} # Include libuv headers
)

# Link uWebSockets to uSockets and other necessary dependencies
target_link_libraries(uwebsockets PRIVATE
    usockets
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
    z
    ${LIBUV_LIBRARIES} # Link against libuv
)

# Build your main executable
add_executable(new_monitor new_monitor.cpp)

# Include directories for the executable
target_include_directories(new_monitor PRIVATE
    "${UWS_DIR}/src"             # for uWS.h
    "${USOCKETS_DIR}/src"        # for uSockets
    # nlohmann/json headers are handled by nlohmann_json::nlohmann_json target
)

# Link the final executable
target_link_libraries(new_monitor PRIVATE
    uwebsockets                  # Link against the newly built uwebsockets library
    usockets                     # Ensure usockets is also linked directly
    pthread
    z
    ${LIBUV_LIBRARIES}           # Link libuv explicitly for the main executable
    nlohmann_json::nlohmann_json # Link against the target provided by find_package(nlohmann_json)
)
