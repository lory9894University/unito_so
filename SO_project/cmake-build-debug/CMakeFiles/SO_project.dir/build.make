# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/clion/98/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/98/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lorenzo/ClionProjects/SO_project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lorenzo/ClionProjects/SO_project/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/SO_project.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/SO_project.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SO_project.dir/flags.make

CMakeFiles/SO_project.dir/master.c.o: CMakeFiles/SO_project.dir/flags.make
CMakeFiles/SO_project.dir/master.c.o: ../master.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lorenzo/ClionProjects/SO_project/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/SO_project.dir/master.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/SO_project.dir/master.c.o   -c /home/lorenzo/ClionProjects/SO_project/master.c

CMakeFiles/SO_project.dir/master.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SO_project.dir/master.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lorenzo/ClionProjects/SO_project/master.c > CMakeFiles/SO_project.dir/master.c.i

CMakeFiles/SO_project.dir/master.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SO_project.dir/master.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lorenzo/ClionProjects/SO_project/master.c -o CMakeFiles/SO_project.dir/master.c.s

CMakeFiles/SO_project.dir/player.c.o: CMakeFiles/SO_project.dir/flags.make
CMakeFiles/SO_project.dir/player.c.o: ../player.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lorenzo/ClionProjects/SO_project/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/SO_project.dir/player.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/SO_project.dir/player.c.o   -c /home/lorenzo/ClionProjects/SO_project/player.c

CMakeFiles/SO_project.dir/player.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SO_project.dir/player.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lorenzo/ClionProjects/SO_project/player.c > CMakeFiles/SO_project.dir/player.c.i

CMakeFiles/SO_project.dir/player.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SO_project.dir/player.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lorenzo/ClionProjects/SO_project/player.c -o CMakeFiles/SO_project.dir/player.c.s

CMakeFiles/SO_project.dir/pawn.c.o: CMakeFiles/SO_project.dir/flags.make
CMakeFiles/SO_project.dir/pawn.c.o: ../pawn.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lorenzo/ClionProjects/SO_project/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/SO_project.dir/pawn.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/SO_project.dir/pawn.c.o   -c /home/lorenzo/ClionProjects/SO_project/pawn.c

CMakeFiles/SO_project.dir/pawn.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SO_project.dir/pawn.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lorenzo/ClionProjects/SO_project/pawn.c > CMakeFiles/SO_project.dir/pawn.c.i

CMakeFiles/SO_project.dir/pawn.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SO_project.dir/pawn.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lorenzo/ClionProjects/SO_project/pawn.c -o CMakeFiles/SO_project.dir/pawn.c.s

CMakeFiles/SO_project.dir/shared_res.c.o: CMakeFiles/SO_project.dir/flags.make
CMakeFiles/SO_project.dir/shared_res.c.o: ../shared_res.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lorenzo/ClionProjects/SO_project/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/SO_project.dir/shared_res.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/SO_project.dir/shared_res.c.o   -c /home/lorenzo/ClionProjects/SO_project/shared_res.c

CMakeFiles/SO_project.dir/shared_res.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SO_project.dir/shared_res.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lorenzo/ClionProjects/SO_project/shared_res.c > CMakeFiles/SO_project.dir/shared_res.c.i

CMakeFiles/SO_project.dir/shared_res.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SO_project.dir/shared_res.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lorenzo/ClionProjects/SO_project/shared_res.c -o CMakeFiles/SO_project.dir/shared_res.c.s

# Object files for target SO_project
SO_project_OBJECTS = \
"CMakeFiles/SO_project.dir/master.c.o" \
"CMakeFiles/SO_project.dir/player.c.o" \
"CMakeFiles/SO_project.dir/pawn.c.o" \
"CMakeFiles/SO_project.dir/shared_res.c.o"

# External object files for target SO_project
SO_project_EXTERNAL_OBJECTS =

SO_project: CMakeFiles/SO_project.dir/master.c.o
SO_project: CMakeFiles/SO_project.dir/player.c.o
SO_project: CMakeFiles/SO_project.dir/pawn.c.o
SO_project: CMakeFiles/SO_project.dir/shared_res.c.o
SO_project: CMakeFiles/SO_project.dir/build.make
SO_project: CMakeFiles/SO_project.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lorenzo/ClionProjects/SO_project/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable SO_project"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SO_project.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SO_project.dir/build: SO_project

.PHONY : CMakeFiles/SO_project.dir/build

CMakeFiles/SO_project.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SO_project.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SO_project.dir/clean

CMakeFiles/SO_project.dir/depend:
	cd /home/lorenzo/ClionProjects/SO_project/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lorenzo/ClionProjects/SO_project /home/lorenzo/ClionProjects/SO_project /home/lorenzo/ClionProjects/SO_project/cmake-build-debug /home/lorenzo/ClionProjects/SO_project/cmake-build-debug /home/lorenzo/ClionProjects/SO_project/cmake-build-debug/CMakeFiles/SO_project.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SO_project.dir/depend

