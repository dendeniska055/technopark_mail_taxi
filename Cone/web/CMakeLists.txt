cmake_minimum_required(VERSION 3.17)
project(cone_web)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
file(GLOB C_SOURCES "src/*.cpp")
include_directories(../CDSHelper/include)
find_package(Boost 1.74.0 REQUIRED)
find_package(LibCDS CONFIG REQUIRED PATHS /home/anton/vcpkg/packages/libcds_x64-linux/share/libcds)
include_directories(${Boost_INCLUDE_DIRS})
add_library(cone_web SHARED ${C_SOURCES})
target_link_libraries(cone_web LibCDS::cds-s)
