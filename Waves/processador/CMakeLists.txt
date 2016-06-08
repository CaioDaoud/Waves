cmake_minimum_required (VERSION 2.4)
project (queryProcessor)

include("${${PROJECT_NAME}_SOURCE_DIR}/globalVariables.cmake")

#set(CMAKE_CXX_FLAGS "--coverage -fprofile-dir=/home/thomaz/Documentos/trabalho/codigoTrabalho/processador/build/Debug/")
#set(CMAKE_SHARED_LINKER_FLAGS "--coverage -fprofile-dir=/home/thomaz/Documentos/trabalho/codigoTrabalho/processador/build/Debug/")

set(CMAKE_CXX_FLAGS "--coverage")
set(CMAKE_SHARED_LINKER_FLAGS "--coverage")

include_directories(${INCLUDES})

link_directories(${LIBS})

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_subdirectory(src)

add_executable(QueryProcessor src/run/queryProcessor.cpp)
target_link_libraries (QueryProcessor libQueryProcessor rt -fprofile-arcs)

add_definitions(-O2 )


