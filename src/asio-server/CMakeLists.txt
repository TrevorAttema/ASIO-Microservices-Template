cmake_minimum_required(VERSION 3.5.1)

include_directories("${PROTOC_OUTPUT_DIR}")

find_package(asio-grpc CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS coroutine thread filesystem program_options)

add_compile_definitions(BOOST_ASIO_USE_TS_EXECUTOR_AS_DEFAULT=1 LIBODB_MYSQL_INCLUDE_LONG=1)

add_executable(asio_server sa_asioserver.cc authServer.h authServer.cpp)
target_link_libraries(asio_server
  sa_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  Boost::program_options
  asio-grpc::asio-grpc
)
