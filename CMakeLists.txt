cmake_minimum_required(VERSION 3.17)
project(untitled1)

set(CMAKE_CXX_STANDARD 20)

set(BOOST_ROOT "/usr/local/Cellar/boost/1.74.0")

#find_package(Boost 1.74.0 COMPONENTS system filesystem REQUIRED)

find_package(Boost)
find_package(PostgreSQL REQUIRED)


if (NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost!")
endif()

include_directories(${Boost_INCLUDE_DIRS})

set(auth "project/src/AuthorizationServer.cpp" "project/src/SHA1.cpp")
set(data project/src/DatabaseManager.cpp project/src/DatabaseConnection.cpp)

add_executable(mail_taxi project/main.cpp ${auth} ${data})

target_include_directories(mail_taxi PRIVATE ${PostgreSQL_INCLUDE_DIRS})

target_link_libraries(mail_taxi ${Boost_LIBRARIES} ${PostgreSQL_LIBRARIES})