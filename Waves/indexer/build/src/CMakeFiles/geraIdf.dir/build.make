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
include src/CMakeFiles/geraIdf.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/geraIdf.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/geraIdf.dir/flags.make

src/CMakeFiles/geraIdf.dir/run/geraIdf.o: src/CMakeFiles/geraIdf.dir/flags.make
src/CMakeFiles/geraIdf.dir/run/geraIdf.o: ../src/run/geraIdf.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/geraIdf.dir/run/geraIdf.o"
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/geraIdf.dir/run/geraIdf.o -c /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/src/run/geraIdf.cc

src/CMakeFiles/geraIdf.dir/run/geraIdf.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/geraIdf.dir/run/geraIdf.i"
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/src/run/geraIdf.cc > CMakeFiles/geraIdf.dir/run/geraIdf.i

src/CMakeFiles/geraIdf.dir/run/geraIdf.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/geraIdf.dir/run/geraIdf.s"
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/src/run/geraIdf.cc -o CMakeFiles/geraIdf.dir/run/geraIdf.s

src/CMakeFiles/geraIdf.dir/run/geraIdf.o.requires:

.PHONY : src/CMakeFiles/geraIdf.dir/run/geraIdf.o.requires

src/CMakeFiles/geraIdf.dir/run/geraIdf.o.provides: src/CMakeFiles/geraIdf.dir/run/geraIdf.o.requires
	$(MAKE) -f src/CMakeFiles/geraIdf.dir/build.make src/CMakeFiles/geraIdf.dir/run/geraIdf.o.provides.build
.PHONY : src/CMakeFiles/geraIdf.dir/run/geraIdf.o.provides

src/CMakeFiles/geraIdf.dir/run/geraIdf.o.provides.build: src/CMakeFiles/geraIdf.dir/run/geraIdf.o


# Object files for target geraIdf
geraIdf_OBJECTS = \
"CMakeFiles/geraIdf.dir/run/geraIdf.o"

# External object files for target geraIdf
geraIdf_EXTERNAL_OBJECTS =

src/geraIdf: src/CMakeFiles/geraIdf.dir/run/geraIdf.o
src/geraIdf: src/CMakeFiles/geraIdf.dir/build.make
src/geraIdf: src/libindexer.so
src/geraIdf: src/CMakeFiles/geraIdf.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable geraIdf"
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/geraIdf.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/geraIdf.dir/build: src/geraIdf

.PHONY : src/CMakeFiles/geraIdf.dir/build

src/CMakeFiles/geraIdf.dir/requires: src/CMakeFiles/geraIdf.dir/run/geraIdf.o.requires

.PHONY : src/CMakeFiles/geraIdf.dir/requires

src/CMakeFiles/geraIdf.dir/clean:
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && $(CMAKE_COMMAND) -P CMakeFiles/geraIdf.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/geraIdf.dir/clean

src/CMakeFiles/geraIdf.dir/depend:
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/src /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src/CMakeFiles/geraIdf.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/geraIdf.dir/depend

