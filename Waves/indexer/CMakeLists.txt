cmake_minimum_required (VERSION 2.4)
project (indexer)

include(${${PROJECT_NAME}_SOURCE_DIR}/globalVariables.cmake)

include_directories(${INCLUDES})

link_directories(${LIBS})

add_subdirectory(src)

add_definitions(-pg -march=pentium4 -msse3 -g)
