# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 4.0

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/hanifadelekan/dev/Trading-Prod/gui

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/hanifadelekan/dev/Trading-Prod/gui/build

# Include any dependencies generated for this target.
include CMakeFiles/main.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/main.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/main.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main.dir/flags.make

CMakeFiles/main.dir/codegen:
.PHONY : CMakeFiles/main.dir/codegen

CMakeFiles/main.dir/gui_main.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/gui_main.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/gui/gui_main.cpp
CMakeFiles/main.dir/gui_main.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/main.dir/gui_main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/gui_main.cpp.o -MF CMakeFiles/main.dir/gui_main.cpp.o.d -o CMakeFiles/main.dir/gui_main.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/gui/gui_main.cpp

CMakeFiles/main.dir/gui_main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/gui_main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/gui/gui_main.cpp > CMakeFiles/main.dir/gui_main.cpp.i

CMakeFiles/main.dir/gui_main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/gui_main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/gui/gui_main.cpp -o CMakeFiles/main.dir/gui_main.cpp.s

CMakeFiles/main.dir/imgui_bbo_viewer.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/imgui_bbo_viewer.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/gui/imgui_bbo_viewer.cpp
CMakeFiles/main.dir/imgui_bbo_viewer.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/main.dir/imgui_bbo_viewer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/imgui_bbo_viewer.cpp.o -MF CMakeFiles/main.dir/imgui_bbo_viewer.cpp.o.d -o CMakeFiles/main.dir/imgui_bbo_viewer.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/gui/imgui_bbo_viewer.cpp

CMakeFiles/main.dir/imgui_bbo_viewer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/imgui_bbo_viewer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/gui/imgui_bbo_viewer.cpp > CMakeFiles/main.dir/imgui_bbo_viewer.cpp.i

CMakeFiles/main.dir/imgui_bbo_viewer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/imgui_bbo_viewer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/gui/imgui_bbo_viewer.cpp -o CMakeFiles/main.dir/imgui_bbo_viewer.cpp.s

CMakeFiles/main.dir/gui_app.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/gui_app.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/gui/gui_app.cpp
CMakeFiles/main.dir/gui_app.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/main.dir/gui_app.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/gui_app.cpp.o -MF CMakeFiles/main.dir/gui_app.cpp.o.d -o CMakeFiles/main.dir/gui_app.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/gui/gui_app.cpp

CMakeFiles/main.dir/gui_app.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/gui_app.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/gui/gui_app.cpp > CMakeFiles/main.dir/gui_app.cpp.i

CMakeFiles/main.dir/gui_app.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/gui_app.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/gui/gui_app.cpp -o CMakeFiles/main.dir/gui_app.cpp.s

CMakeFiles/main.dir/websocket_receivers.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/websocket_receivers.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/gui/websocket_receivers.cpp
CMakeFiles/main.dir/websocket_receivers.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/main.dir/websocket_receivers.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/websocket_receivers.cpp.o -MF CMakeFiles/main.dir/websocket_receivers.cpp.o.d -o CMakeFiles/main.dir/websocket_receivers.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/gui/websocket_receivers.cpp

CMakeFiles/main.dir/websocket_receivers.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/websocket_receivers.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/gui/websocket_receivers.cpp > CMakeFiles/main.dir/websocket_receivers.cpp.i

CMakeFiles/main.dir/websocket_receivers.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/websocket_receivers.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/gui/websocket_receivers.cpp -o CMakeFiles/main.dir/websocket_receivers.cpp.s

CMakeFiles/main.dir/third_party/imgui/imgui.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/third_party/imgui/imgui.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui.cpp
CMakeFiles/main.dir/third_party/imgui/imgui.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/main.dir/third_party/imgui/imgui.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/third_party/imgui/imgui.cpp.o -MF CMakeFiles/main.dir/third_party/imgui/imgui.cpp.o.d -o CMakeFiles/main.dir/third_party/imgui/imgui.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui.cpp

CMakeFiles/main.dir/third_party/imgui/imgui.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/third_party/imgui/imgui.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui.cpp > CMakeFiles/main.dir/third_party/imgui/imgui.cpp.i

CMakeFiles/main.dir/third_party/imgui/imgui.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/third_party/imgui/imgui.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui.cpp -o CMakeFiles/main.dir/third_party/imgui/imgui.cpp.s

CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui_draw.cpp
CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.o -MF CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.o.d -o CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui_draw.cpp

CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui_draw.cpp > CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.i

CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui_draw.cpp -o CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.s

CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui_widgets.cpp
CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.o -MF CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.o.d -o CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui_widgets.cpp

CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui_widgets.cpp > CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.i

CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui_widgets.cpp -o CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.s

CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui_tables.cpp
CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.o -MF CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.o.d -o CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui_tables.cpp

CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui_tables.cpp > CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.i

CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/imgui_tables.cpp -o CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.s

CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/backends/imgui_impl_glfw.cpp
CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.o -MF CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.o.d -o CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/backends/imgui_impl_glfw.cpp

CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/backends/imgui_impl_glfw.cpp > CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.i

CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/backends/imgui_impl_glfw.cpp -o CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.s

CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/backends/imgui_impl_opengl3.cpp
CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.o -MF CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.o.d -o CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/backends/imgui_impl_opengl3.cpp

CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/backends/imgui_impl_opengl3.cpp > CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.i

CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/backends/imgui_impl_opengl3.cpp -o CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.s

CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/misc/cpp/imgui_stdlib.cpp
CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.o -MF CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.o.d -o CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/misc/cpp/imgui_stdlib.cpp

CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/misc/cpp/imgui_stdlib.cpp > CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.i

CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/imgui/misc/cpp/imgui_stdlib.cpp -o CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.s

CMakeFiles/main.dir/third_party/implot/implot.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/third_party/implot/implot.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/implot/implot.cpp
CMakeFiles/main.dir/third_party/implot/implot.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/main.dir/third_party/implot/implot.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/third_party/implot/implot.cpp.o -MF CMakeFiles/main.dir/third_party/implot/implot.cpp.o.d -o CMakeFiles/main.dir/third_party/implot/implot.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/implot/implot.cpp

CMakeFiles/main.dir/third_party/implot/implot.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/third_party/implot/implot.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/implot/implot.cpp > CMakeFiles/main.dir/third_party/implot/implot.cpp.i

CMakeFiles/main.dir/third_party/implot/implot.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/third_party/implot/implot.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/implot/implot.cpp -o CMakeFiles/main.dir/third_party/implot/implot.cpp.s

CMakeFiles/main.dir/third_party/implot/implot_items.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/third_party/implot/implot_items.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/implot/implot_items.cpp
CMakeFiles/main.dir/third_party/implot/implot_items.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/main.dir/third_party/implot/implot_items.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/third_party/implot/implot_items.cpp.o -MF CMakeFiles/main.dir/third_party/implot/implot_items.cpp.o.d -o CMakeFiles/main.dir/third_party/implot/implot_items.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/implot/implot_items.cpp

CMakeFiles/main.dir/third_party/implot/implot_items.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/third_party/implot/implot_items.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/implot/implot_items.cpp > CMakeFiles/main.dir/third_party/implot/implot_items.cpp.i

CMakeFiles/main.dir/third_party/implot/implot_items.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/third_party/implot/implot_items.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/gui/third_party/implot/implot_items.cpp -o CMakeFiles/main.dir/third_party/implot/implot_items.cpp.s

# Object files for target main
main_OBJECTS = \
"CMakeFiles/main.dir/gui_main.cpp.o" \
"CMakeFiles/main.dir/imgui_bbo_viewer.cpp.o" \
"CMakeFiles/main.dir/gui_app.cpp.o" \
"CMakeFiles/main.dir/websocket_receivers.cpp.o" \
"CMakeFiles/main.dir/third_party/imgui/imgui.cpp.o" \
"CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.o" \
"CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.o" \
"CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.o" \
"CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.o" \
"CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.o" \
"CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.o" \
"CMakeFiles/main.dir/third_party/implot/implot.cpp.o" \
"CMakeFiles/main.dir/third_party/implot/implot_items.cpp.o"

# External object files for target main
main_EXTERNAL_OBJECTS =

main: CMakeFiles/main.dir/gui_main.cpp.o
main: CMakeFiles/main.dir/imgui_bbo_viewer.cpp.o
main: CMakeFiles/main.dir/gui_app.cpp.o
main: CMakeFiles/main.dir/websocket_receivers.cpp.o
main: CMakeFiles/main.dir/third_party/imgui/imgui.cpp.o
main: CMakeFiles/main.dir/third_party/imgui/imgui_draw.cpp.o
main: CMakeFiles/main.dir/third_party/imgui/imgui_widgets.cpp.o
main: CMakeFiles/main.dir/third_party/imgui/imgui_tables.cpp.o
main: CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_glfw.cpp.o
main: CMakeFiles/main.dir/third_party/imgui/backends/imgui_impl_opengl3.cpp.o
main: CMakeFiles/main.dir/third_party/imgui/misc/cpp/imgui_stdlib.cpp.o
main: CMakeFiles/main.dir/third_party/implot/implot.cpp.o
main: CMakeFiles/main.dir/third_party/implot/implot_items.cpp.o
main: CMakeFiles/main.dir/build.make
main: /opt/homebrew/lib/libglfw.3.4.dylib
main: /opt/anaconda3/lib/libhdf5_cpp.310.0.5.dylib
main: /opt/anaconda3/lib/libhdf5.310.5.0.dylib
main: CMakeFiles/main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Linking CXX executable main"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main.dir/build: main
.PHONY : CMakeFiles/main.dir/build

CMakeFiles/main.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main.dir/clean

CMakeFiles/main.dir/depend:
	cd /Users/hanifadelekan/dev/Trading-Prod/gui/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/hanifadelekan/dev/Trading-Prod/gui /Users/hanifadelekan/dev/Trading-Prod/gui /Users/hanifadelekan/dev/Trading-Prod/gui/build /Users/hanifadelekan/dev/Trading-Prod/gui/build /Users/hanifadelekan/dev/Trading-Prod/gui/build/CMakeFiles/main.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/main.dir/depend

