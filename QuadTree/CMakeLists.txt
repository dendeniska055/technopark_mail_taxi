cmake_minimum_required(VERSION 3.17)
project(QuadTree)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
add_executable(QuadTree main.cpp include/WorldQuadTree.h)
