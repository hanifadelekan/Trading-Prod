cmake_minimum_required(VERSION 3.10)
project(trader CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(IMGUI_ROOT_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)
set(IMPLOT_ROOT_DIR ${CMAKE_SOURCE_DIR}/third_party/implot)

set(APP_SOURCES
    main.cpp
    gui/imgui_bbo_viewer.cpp
    core/networking/resolve_ws.cpp
    core/networking/ws_reader.cpp        # NEW: your orderbook feed
    gui/gui_app.cpp
    exchanges/hyperliquid/global_atomics.cpp
    exchanges/hyperliquid/parsing/hl_parsers.cpp
    exchanges/hyperliquid/streaming/subscription_patterns.cpp
    exchanges/hyperliquid/streaming/threads.cpp
    datacentre/strat_ctrl.cpp
    datacentre/control.cpp

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
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/exchanges
    ${CMAKE_SOURCE_DIR}/gui
    ${IMGUI_ROOT_DIR}
    ${IMGUI_ROOT_DIR}/backends
    ${IMPLOT_ROOT_DIR}
    ${CMAKE_SOURCE_DIR}
    ${HDF5_INCLUDE_DIRS}

    /opt/homebrew/include
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(HDF5 REQUIRED COMPONENTS C CXX)

target_link_libraries(main
    glfw
    ${OPENGL_gl_LIBRARY}
    boost_system
    ssl
    crypto
    ${HDF5_C_LIBRARIES}
    ${HDF5_CXX_LIBRARIES}
)

target_compile_definitions(main PRIVATE GL_SILENCE_DEPRECATION)
target_compile_options(main PRIVATE -O3 -march=native)
