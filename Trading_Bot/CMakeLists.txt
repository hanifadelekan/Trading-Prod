cmake_minimum_required(VERSION 3.10)
project(hyperliquid_bbo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Boost headers
include_directories(/opt/homebrew/include)  # or /usr/local/include on Intel Mac

# Add executable from source file
add_executable(hyperliquid_bbo hyperliquid_bbo.cpp)

# Tell CMake to link libraries needed for Boost.Beast + SSL
target_link_libraries(hyperliquid_bbo boost_system ssl crypto)

# Optionally set compiler optimization flags
target_compile_options(hyperliquid_bbo PRIVATE -O3 -march=native)
