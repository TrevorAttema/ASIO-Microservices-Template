# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.23)

project ("services")

set(CMAKE_CXX_STANDARD 20)

message(STATUS "VCPKG_TARGET_TRIPLET = ${VCPKG_TARGET_TRIPLET}")
set(PROTOC_OUTPUT_DIR ${CMAKE_BINARY_DIR}/common)

# Include sub-projects.
add_subdirectory ("proto")
add_subdirectory ("client")
add_subdirectory ("server")
add_subdirectory ("asio-server")
add_subdirectory ("async-server")

