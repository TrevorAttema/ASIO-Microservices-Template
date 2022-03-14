cmake_minimum_required(VERSION 3.5.1)

include_directories("${PROTOC_OUTPUT_DIR}")

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(sa_client sa_client.cc SAClient.h SAClient.cpp)
target_link_libraries(sa_client
  sa_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  Boost::program_options
)
