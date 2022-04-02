cmake_minimum_required(VERSION 3.5.1)

include_directories("${PROTOC_OUTPUT_DIR}")

find_package(Boost REQUIRED COMPONENTS thread filesystem program_options)

add_executable(async_server async_server.cc authServer.cpp authServer.h)
target_link_libraries(async_server
  sa_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  Boost::program_options
  )
