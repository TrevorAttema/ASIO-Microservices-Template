cmake_minimum_required(VERSION 3.5.1)

include_directories("${PROTOC_OUTPUT_DIR}")

add_executable(sa_server sa_server.cc)
target_link_libraries(sa_server
  sa_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
