cmake_minimum_required(VERSION 3.17)
project(GeoIndex)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -pthread")
include_directories(/home/anton/dev/techno_park/cpp/PROJ/libs/boost_1_74_0/boost/)
include_directories(include)
add_executable(GeoIndex src/main.cpp include/ICashTable.h include/IGeoIndexer.h include/Server.h)
target_link_libraries(GeoIndex PRIVATE /home/anton/dev/techno_park/cpp/PROJ/libs/boost_1_74_0/stage/lib/libboost_thread.a)