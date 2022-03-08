cmake_minimum_required(VERSION 3.5.1)

include_directories("${PROTOC_OUTPUT_DIR}")

message(STATUS "CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}")
find_package(asio-grpc CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS coroutine thread filesystem)

add_executable(asio_server sa_asioserver.cc)
target_link_libraries(asio_server
  sa_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  asio-grpc::asio-grpc
)
