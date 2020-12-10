cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)
project(GeoIndex)

find_package(Boost 1.74.0 REQUIRED thread )
find_package(LibCDS CONFIG REQUIRED PATHS /home/anton/vcpkg/packages/libcds_x64-linux/share/libcds)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Threads REQUIRED)
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()


find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

get_filename_component(proto_file "../Description_of_external_interfaces/GeoIndex.proto" ABSOLUTE)
get_filename_component(proto_file_path "${proto_file}" PATH)


set(proto_src "${CMAKE_CURRENT_BINARY_DIR}/GeoIndex.pb.cc")
set(proto_h "${CMAKE_CURRENT_BINARY_DIR}/GeoIndex.pb.h")
set(grpc_src "${CMAKE_CURRENT_BINARY_DIR}/GeoIndex.grpc.pb.cc")
set(grpc_h "${CMAKE_CURRENT_BINARY_DIR}/GeoIndex.grpc.pb.h")

add_custom_command(
        OUTPUT "${proto_src}" "${proto_h}" "${grpc_src}" "${grpc_h}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${proto_file_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${proto_file}"
        DEPENDS "${proto_file}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(../Cone/QuadTree/include)
include_directories(../Cone/web/include)
include_directories(../Cone/CDSHelper/include)
include_directories(include )
file(GLOB C_SOURCES "src/*.cpp")
add_executable(GeoIndex ${proto_src} ${grpc_src} ${C_SOURCES} include/DriverClientConnector.h)
    target_link_libraries(GeoIndex Boost::thread LibCDS::cds-s /home/anton/dev/techno_park/geo_index_service/Cone/web/Build/libcone_web.so
            /home/anton/dev/techno_park/geo_index_service/Cone/QuadTree/cmake-build-debug/libcone_quadTree.so
            ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})