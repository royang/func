# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/roy/Documents/test/cmake

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/roy/Documents/test/cmake/build

# Include any dependencies generated for this target.
include MathFunc/CMakeFiles/MathFunctions,.dir/depend.make

# Include the progress variables for this target.
include MathFunc/CMakeFiles/MathFunctions,.dir/progress.make

# Include the compile flags for this target's objects.
include MathFunc/CMakeFiles/MathFunctions,.dir/flags.make

MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.o: MathFunc/CMakeFiles/MathFunctions,.dir/flags.make
MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.o: ../MathFunc/sip_math.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/roy/Documents/test/cmake/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.o"
	cd /home/roy/Documents/test/cmake/build/MathFunc && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/MathFunctions,.dir/sip_math.c.o   -c /home/roy/Documents/test/cmake/MathFunc/sip_math.c

MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MathFunctions,.dir/sip_math.c.i"
	cd /home/roy/Documents/test/cmake/build/MathFunc && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/roy/Documents/test/cmake/MathFunc/sip_math.c > CMakeFiles/MathFunctions,.dir/sip_math.c.i

MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MathFunctions,.dir/sip_math.c.s"
	cd /home/roy/Documents/test/cmake/build/MathFunc && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/roy/Documents/test/cmake/MathFunc/sip_math.c -o CMakeFiles/MathFunctions,.dir/sip_math.c.s

MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.o.requires:
.PHONY : MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.o.requires

MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.o.provides: MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.o.requires
	$(MAKE) -f MathFunc/CMakeFiles/MathFunctions,.dir/build.make MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.o.provides.build
.PHONY : MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.o.provides

MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.o.provides.build: MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.o

# Object files for target MathFunctions,
MathFunctions,_OBJECTS = \
"CMakeFiles/MathFunctions,.dir/sip_math.c.o"

# External object files for target MathFunctions,
MathFunctions,_EXTERNAL_OBJECTS =

MathFunc/libMathFunctions,.a: MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.o
MathFunc/libMathFunctions,.a: MathFunc/CMakeFiles/MathFunctions,.dir/build.make
MathFunc/libMathFunctions,.a: MathFunc/CMakeFiles/MathFunctions,.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C static library libMathFunctions,.a"
	cd /home/roy/Documents/test/cmake/build/MathFunc && $(CMAKE_COMMAND) -P CMakeFiles/MathFunctions,.dir/cmake_clean_target.cmake
	cd /home/roy/Documents/test/cmake/build/MathFunc && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MathFunctions,.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
MathFunc/CMakeFiles/MathFunctions,.dir/build: MathFunc/libMathFunctions,.a
.PHONY : MathFunc/CMakeFiles/MathFunctions,.dir/build

MathFunc/CMakeFiles/MathFunctions,.dir/requires: MathFunc/CMakeFiles/MathFunctions,.dir/sip_math.c.o.requires
.PHONY : MathFunc/CMakeFiles/MathFunctions,.dir/requires

MathFunc/CMakeFiles/MathFunctions,.dir/clean:
	cd /home/roy/Documents/test/cmake/build/MathFunc && $(CMAKE_COMMAND) -P CMakeFiles/MathFunctions,.dir/cmake_clean.cmake
.PHONY : MathFunc/CMakeFiles/MathFunctions,.dir/clean

MathFunc/CMakeFiles/MathFunctions,.dir/depend:
	cd /home/roy/Documents/test/cmake/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/roy/Documents/test/cmake /home/roy/Documents/test/cmake/MathFunc /home/roy/Documents/test/cmake/build /home/roy/Documents/test/cmake/build/MathFunc /home/roy/Documents/test/cmake/build/MathFunc/CMakeFiles/MathFunctions,.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : MathFunc/CMakeFiles/MathFunctions,.dir/depend
