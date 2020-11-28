cmake_minimum_required(VERSION 3.17)
project(project)

set(CMAKE_CXX_STANDARD 17)

set(SRC project/main.cpp project/src/User_Authorization.cpp project/src/Handler.cpp project/src/Request.cpp project/src/User_Registration.cpp)

add_executable(project ${SRC})