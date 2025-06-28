cmake_minimum_required(VERSION 3.10)
project(trader CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(IMGUI_ROOT_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)
set(IMPLOT_ROOT_DIR ${CMAKE_SOURCE_DIR}/third_party/implot)

set(APP_SOURCES
    main.cpp
    imgui_bbo_viewer.cpp
    hyperliquid_bbo.cpp
    hyperliquid_orderbook.cpp        # NEW: your orderbook feed
)

set(IMGUI_CORE_SOURCES
    ${IMGUI_ROOT_DIR}/imgui.cpp
    ${IMGUI_ROOT_DIR}/imgui_draw.cpp
    ${IMGUI_ROOT_DIR}/imgui_widgets.cpp
    ${IMGUI_ROOT_DIR}/imgui_tables.cpp
)

set(IMGUI_BACKEND_SOURCES
    ${IMGUI_ROOT_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_ROOT_DIR}/backends/imgui_impl_opengl3.cpp
)

set(IMGUI_MISC_SOURCES
    ${IMGUI_ROOT_DIR}/misc/cpp/imgui_stdlib.cpp
)

set(IMPLOT_SOURCES
    ${IMPLOT_ROOT_DIR}/implot.cpp
    ${IMPLOT_ROOT_DIR}/implot_items.cpp
)

add_executable(main
    ${APP_SOURCES}
    ${IMGUI_CORE_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
    ${IMGUI_MISC_SOURCES}
    ${IMPLOT_SOURCES}
)

target_include_directories(main PRIVATE
    ${IMGUI_ROOT_DIR}
    ${IMGUI_ROOT_DIR}/backends
    ${IMPLOT_ROOT_DIR}
    ${CMAKE_SOURCE_DIR}
    /opt/homebrew/include
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(main
    glfw
    ${OPENGL_gl_LIBRARY}
    boost_system
    ssl
    crypto
)

target_compile_definitions(main PRIVATE GL_SILENCE_DEPRECATION)
target_compile_options(main PRIVATE -O3 -march=native)
