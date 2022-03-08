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

  # Find gRPC installation
  # Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
  find_package(gRPC CONFIG REQUIRED)
  message(STATUS "Using gRPC ${gRPC_VERSION}")

  set(_GRPC_GRPCPP gRPC::grpc++)
  if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
  else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
  endif()

# Proto file
get_filename_component(sa_proto "services.accounts.proto" ABSOLUTE)
get_filename_component(sa_proto_path "${sa_proto}" PATH)

# Generated sources
set(sa_proto_srcs "${PROTOC_OUTPUT_DIR}/services.accounts.pb.cc")
set(sa_proto_hdrs "${PROTOC_OUTPUT_DIR}/services.accounts.pb.h")
set(sa_grpc_srcs "${PROTOC_OUTPUT_DIR}/services.accounts.grpc.pb.cc")
set(sa_grpc_hdrs "${PROTOC_OUTPUT_DIR}/services.accounts.grpc.pb.h")
set(PROTOC_OUTPUTS "${sa_proto_srcs}" "${sa_proto_hdrs}" "${sa_grpc_srcs}" "${sa_grpc_hdrs}")

add_custom_command(
      OUTPUT ${PROTOC_OUTPUTS}
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${PROTOC_OUTPUT_DIR}"
        --cpp_out "${PROTOC_OUTPUT_DIR}"
        -I "${sa_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${sa_proto}"
      MAIN_DEPENDENCY "${sa_proto}"
      SOURCE "${sa_proto}")

# sa_grpc_proto
add_library(sa_grpc_proto
  ${PROTOC_OUTPUTS})
target_link_libraries(sa_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
