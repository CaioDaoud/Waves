# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build

# Include any dependencies generated for this target.
include src/CMakeFiles/geraParcial.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/geraParcial.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/geraParcial.dir/flags.make

src/CMakeFiles/geraParcial.dir/run/geraParcial.o: src/CMakeFiles/geraParcial.dir/flags.make
src/CMakeFiles/geraParcial.dir/run/geraParcial.o: ../src/run/geraParcial.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/geraParcial.dir/run/geraParcial.o"
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/geraParcial.dir/run/geraParcial.o -c /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/src/run/geraParcial.cc

src/CMakeFiles/geraParcial.dir/run/geraParcial.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/geraParcial.dir/run/geraParcial.i"
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/src/run/geraParcial.cc > CMakeFiles/geraParcial.dir/run/geraParcial.i

src/CMakeFiles/geraParcial.dir/run/geraParcial.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/geraParcial.dir/run/geraParcial.s"
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/src/run/geraParcial.cc -o CMakeFiles/geraParcial.dir/run/geraParcial.s

src/CMakeFiles/geraParcial.dir/run/geraParcial.o.requires:

.PHONY : src/CMakeFiles/geraParcial.dir/run/geraParcial.o.requires

src/CMakeFiles/geraParcial.dir/run/geraParcial.o.provides: src/CMakeFiles/geraParcial.dir/run/geraParcial.o.requires
	$(MAKE) -f src/CMakeFiles/geraParcial.dir/build.make src/CMakeFiles/geraParcial.dir/run/geraParcial.o.provides.build
.PHONY : src/CMakeFiles/geraParcial.dir/run/geraParcial.o.provides

src/CMakeFiles/geraParcial.dir/run/geraParcial.o.provides.build: src/CMakeFiles/geraParcial.dir/run/geraParcial.o


# Object files for target geraParcial
geraParcial_OBJECTS = \
"CMakeFiles/geraParcial.dir/run/geraParcial.o"

# External object files for target geraParcial
geraParcial_EXTERNAL_OBJECTS =

src/geraParcial: src/CMakeFiles/geraParcial.dir/run/geraParcial.o
src/geraParcial: src/CMakeFiles/geraParcial.dir/build.make
src/geraParcial: src/libindexer.so
src/geraParcial: src/CMakeFiles/geraParcial.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable geraParcial"
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/geraParcial.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/geraParcial.dir/build: src/geraParcial

.PHONY : src/CMakeFiles/geraParcial.dir/build

src/CMakeFiles/geraParcial.dir/requires: src/CMakeFiles/geraParcial.dir/run/geraParcial.o.requires

.PHONY : src/CMakeFiles/geraParcial.dir/requires

src/CMakeFiles/geraParcial.dir/clean:
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && $(CMAKE_COMMAND) -P CMakeFiles/geraParcial.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/geraParcial.dir/clean

src/CMakeFiles/geraParcial.dir/depend:
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/src /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src/CMakeFiles/geraParcial.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/geraParcial.dir/depend

