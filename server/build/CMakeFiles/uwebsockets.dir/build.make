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
CMAKE_SOURCE_DIR = /Users/hanifadelekan/dev/Trading-Prod/server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/hanifadelekan/dev/Trading-Prod/server/build

# Include any dependencies generated for this target.
include CMakeFiles/uwebsockets.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/uwebsockets.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/uwebsockets.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/uwebsockets.dir/flags.make

CMakeFiles/uwebsockets.dir/codegen:
.PHONY : CMakeFiles/uwebsockets.dir/codegen

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.o: CMakeFiles/uwebsockets.dir/flags.make
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Epoll.cpp
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.o: CMakeFiles/uwebsockets.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.o -MF CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.o.d -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Epoll.cpp

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Epoll.cpp > CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.i

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Epoll.cpp -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.s

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.o: CMakeFiles/uwebsockets.dir/flags.make
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Extensions.cpp
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.o: CMakeFiles/uwebsockets.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.o -MF CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.o.d -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Extensions.cpp

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Extensions.cpp > CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.i

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Extensions.cpp -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.s

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.o: CMakeFiles/uwebsockets.dir/flags.make
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Group.cpp
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.o: CMakeFiles/uwebsockets.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.o -MF CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.o.d -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Group.cpp

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Group.cpp > CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.i

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Group.cpp -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.s

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.o: CMakeFiles/uwebsockets.dir/flags.make
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/HTTPSocket.cpp
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.o: CMakeFiles/uwebsockets.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.o -MF CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.o.d -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/HTTPSocket.cpp

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/HTTPSocket.cpp > CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.i

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/HTTPSocket.cpp -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.s

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.o: CMakeFiles/uwebsockets.dir/flags.make
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Hub.cpp
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.o: CMakeFiles/uwebsockets.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.o -MF CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.o.d -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Hub.cpp

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Hub.cpp > CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.i

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Hub.cpp -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.s

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.o: CMakeFiles/uwebsockets.dir/flags.make
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Networking.cpp
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.o: CMakeFiles/uwebsockets.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.o -MF CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.o.d -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Networking.cpp

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Networking.cpp > CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.i

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Networking.cpp -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.s

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.o: CMakeFiles/uwebsockets.dir/flags.make
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Node.cpp
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.o: CMakeFiles/uwebsockets.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.o -MF CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.o.d -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Node.cpp

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Node.cpp > CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.i

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Node.cpp -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.s

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.o: CMakeFiles/uwebsockets.dir/flags.make
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Socket.cpp
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.o: CMakeFiles/uwebsockets.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.o -MF CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.o.d -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Socket.cpp

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Socket.cpp > CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.i

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/Socket.cpp -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.s

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.o: CMakeFiles/uwebsockets.dir/flags.make
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.o: /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/WebSocket.cpp
CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.o: CMakeFiles/uwebsockets.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.o -MF CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.o.d -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.o -c /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/WebSocket.cpp

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/WebSocket.cpp > CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.i

CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/hanifadelekan/dev/Trading-Prod/server/uWebSockets/uWebSockets/src/WebSocket.cpp -o CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.s

# Object files for target uwebsockets
uwebsockets_OBJECTS = \
"CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.o" \
"CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.o" \
"CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.o" \
"CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.o" \
"CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.o" \
"CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.o" \
"CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.o" \
"CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.o" \
"CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.o"

# External object files for target uwebsockets
uwebsockets_EXTERNAL_OBJECTS =

libuwebsockets.a: CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Epoll.cpp.o
libuwebsockets.a: CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Extensions.cpp.o
libuwebsockets.a: CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Group.cpp.o
libuwebsockets.a: CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/HTTPSocket.cpp.o
libuwebsockets.a: CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Hub.cpp.o
libuwebsockets.a: CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Networking.cpp.o
libuwebsockets.a: CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Node.cpp.o
libuwebsockets.a: CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/Socket.cpp.o
libuwebsockets.a: CMakeFiles/uwebsockets.dir/uWebSockets/uWebSockets/src/WebSocket.cpp.o
libuwebsockets.a: CMakeFiles/uwebsockets.dir/build.make
libuwebsockets.a: CMakeFiles/uwebsockets.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/hanifadelekan/dev/Trading-Prod/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking CXX static library libuwebsockets.a"
	$(CMAKE_COMMAND) -P CMakeFiles/uwebsockets.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/uwebsockets.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/uwebsockets.dir/build: libuwebsockets.a
.PHONY : CMakeFiles/uwebsockets.dir/build

CMakeFiles/uwebsockets.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/uwebsockets.dir/cmake_clean.cmake
.PHONY : CMakeFiles/uwebsockets.dir/clean

CMakeFiles/uwebsockets.dir/depend:
	cd /Users/hanifadelekan/dev/Trading-Prod/server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/hanifadelekan/dev/Trading-Prod/server /Users/hanifadelekan/dev/Trading-Prod/server /Users/hanifadelekan/dev/Trading-Prod/server/build /Users/hanifadelekan/dev/Trading-Prod/server/build /Users/hanifadelekan/dev/Trading-Prod/server/build/CMakeFiles/uwebsockets.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/uwebsockets.dir/depend

