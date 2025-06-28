cmake_minimum_required(VERSION 3.10)
project(trader CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Prefer strict C++ standard

# Define a variable for the ImGui directory for cleaner paths
set(IMGUI_ROOT_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)

# Define all source files for your executable
# List all .cpp files explicitly. Headers are *not* included here.
set(APP_SOURCES
    main.cpp
    imgui_bbo_viewer.cpp
    hyperliquid_bbo.cpp
)

# Define ImGui source files
set(IMGUI_CORE_SOURCES
    ${IMGUI_ROOT_DIR}/imgui.cpp
    ${IMGUI_ROOT_DIR}/imgui_draw.cpp
    ${IMGUI_ROOT_DIR}/imgui_widgets.cpp
    ${IMGUI_ROOT_DIR}/imgui_tables.cpp # <--- ADD THIS LINE
)

set(IMGUI_BACKEND_SOURCES
    ${IMGUI_ROOT_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_ROOT_DIR}/backends/imgui_impl_opengl3.cpp
)

set(IMGUI_MISC_SOURCES
    ${IMGUI_ROOT_DIR}/misc/cpp/imgui_stdlib.cpp
)

# Combine all source files for the executable
# THIS MUST COME BEFORE ANY target_*_directories/libraries/options/definitions CALLS FOR 'main'
add_executable(main
    ${APP_SOURCES}
    ${IMGUI_CORE_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
    ${IMGUI_MISC_SOURCES}
)

# Add include directories
target_include_directories(main PRIVATE
    ${IMGUI_ROOT_DIR}
    ${IMGUI_ROOT_DIR}/backends
    ${CMAKE_SOURCE_DIR}
    /opt/homebrew/include
)

# Find OpenGL and glfw
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Link all dependencies
target_link_libraries(main
    glfw
    ${OPENGL_gl_LIBRARY}
    boost_system
    ssl
    crypto
)

# Silence macOS OpenGL deprecation warnings
target_compile_definitions(main PRIVATE GL_SILENCE_DEPRECATION)

# Optionally set optimization flags
target_compile_options(main PRIVATE -O3 -march=native)