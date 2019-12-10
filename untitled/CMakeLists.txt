cmake_minimum_required(VERSION 3.15)
project(untitled)

link_libraries(psapi.lib)
set(CMAKE_CXX_STANDARD 14)

add_executable(untitled  Graph.cpp Node.cpp  cstest.cpp )