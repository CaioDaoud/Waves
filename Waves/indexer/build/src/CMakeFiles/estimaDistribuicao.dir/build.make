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
include src/CMakeFiles/estimaDistribuicao.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/estimaDistribuicao.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/estimaDistribuicao.dir/flags.make

src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o: src/CMakeFiles/estimaDistribuicao.dir/flags.make
src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o: ../src/run/estimaDistribuicao.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o"
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o -c /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/src/run/estimaDistribuicao.cc

src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.i"
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/src/run/estimaDistribuicao.cc > CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.i

src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.s"
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/src/run/estimaDistribuicao.cc -o CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.s

src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o.requires:

.PHONY : src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o.requires

src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o.provides: src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o.requires
	$(MAKE) -f src/CMakeFiles/estimaDistribuicao.dir/build.make src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o.provides.build
.PHONY : src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o.provides

src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o.provides.build: src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o


# Object files for target estimaDistribuicao
estimaDistribuicao_OBJECTS = \
"CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o"

# External object files for target estimaDistribuicao
estimaDistribuicao_EXTERNAL_OBJECTS =

src/estimaDistribuicao: src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o
src/estimaDistribuicao: src/CMakeFiles/estimaDistribuicao.dir/build.make
src/estimaDistribuicao: src/libindexer.so
src/estimaDistribuicao: src/CMakeFiles/estimaDistribuicao.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable estimaDistribuicao"
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/estimaDistribuicao.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/estimaDistribuicao.dir/build: src/estimaDistribuicao

.PHONY : src/CMakeFiles/estimaDistribuicao.dir/build

src/CMakeFiles/estimaDistribuicao.dir/requires: src/CMakeFiles/estimaDistribuicao.dir/run/estimaDistribuicao.o.requires

.PHONY : src/CMakeFiles/estimaDistribuicao.dir/requires

src/CMakeFiles/estimaDistribuicao.dir/clean:
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src && $(CMAKE_COMMAND) -P CMakeFiles/estimaDistribuicao.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/estimaDistribuicao.dir/clean

src/CMakeFiles/estimaDistribuicao.dir/depend:
	cd /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/src /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src /home/caio/Documentos/Doutorado/Codigos/gitHub/Waves/indexer/build/src/CMakeFiles/estimaDistribuicao.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/estimaDistribuicao.dir/depend

