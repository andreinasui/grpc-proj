# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(
    FATAL_ERROR
      "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(
  GRPC-PROJECT
  LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options for configuring the build
set(BUILD_SERVICE "ALL" CACHE STRING "What service to build. Valid options are A, B, C, or ALL (default)")

set(EXTERNAL_DEPENDENCIES_PATH
      ${PROJECT_SOURCE_DIR}/ExternalDependencies/installed CACHE STRING "Path to external dependencies")
set(EXTERNAL_GRPC_PATH
      "" CACHE STRING "Path to external grpc dependency")
set(EXTERNAL_PROTOBUF_PATH
      "" CACHE STRING "Path to external protobuf dependency")

# GRPC and Protocol Buffers libraries location
list(APPEND CMAKE_PREFIX_PATH ${EXTERNAL_DEPENDENCIES_PATH} ${EXTERNAL_GRPC_PATH} ${EXTERNAL_PROTOBUF_PATH} ${EXTERNAL_ABSL_PATH} ${EXTERNAL_UTF8_PATH})
list(APPEND CMAKE_MODULE_PATH ${EXTERNAL_DEPENDENCIES_PATH})

find_package(absl CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(opentelemetry-cpp CONFIG REQUIRED)

set(PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
set(GRPC_CPP_PROTOC_PLUGIN $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

set(PROTO_SOURCES ${PROJECT_SOURCE_DIR}/protos/do_work.proto)

set(PROTO_GEN_SRCS ${PROJECT_SOURCE_DIR}/generated/do_work.pb.cc)
set(PROTO_GEN_HDRS ${PROJECT_SOURCE_DIR}/generated/do_work.pb.h)
set(GRPC_GEN_SRCS ${PROJECT_SOURCE_DIR}/generated/do_work.grpc.pb.cc)
set(GRPC_GEN_HDRS ${PROJECT_SOURCE_DIR}/generated/do_work.grpc.pb.h)

add_custom_target(make-generated-dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/generated)
add_custom_command(
  OUTPUT ${PROTO_GEN_SRCS} ${PROTO_GEN_HDRS} ${GRPC_GEN_SRCS} ${GRPC_GEN_HDRS}
  COMMAND ${PROTOBUF_PROTOC} 
  ARGS --cpp_out=${PROJECT_SOURCE_DIR}/generated
    --grpc_out=${PROJECT_SOURCE_DIR}/generated 
    --proto_path=${PROJECT_SOURCE_DIR}/protos
    --plugin=protoc-gen-grpc=${GRPC_CPP_PROTOC_PLUGIN}
    ${PROTO_SOURCES}
    DEPENDS ${PROTO_SOURCES} make-generated-dir
)

add_library(grpc_proto
  ${GRPC_GEN_SRCS}
  ${GRPC_GEN_HDRS}
  ${PROTO_GEN_SRCS}
  ${PROTO_GEN_HDRS})
target_link_libraries(grpc_proto PUBLIC
  gRPC::grpc++_reflection
  gRPC::grpc++
  protobuf::libprotobuf)

target_include_directories(grpc_proto PUBLIC ${PROJECT_SOURCE_DIR}/generated)

if(REMOTE_SERVICES STREQUAL ON)
  add_definitions(-DREMOTE_SERVICES)
endif()

if(BUILD_SERVICE STREQUAL "A" OR BUILD_SERVICE STREQUAL "ALL")
  add_subdirectory(serviceA)
endif()
if(BUILD_SERVICE STREQUAL "B" OR BUILD_SERVICE STREQUAL "ALL")
  add_subdirectory(serviceB)
endif()
if(BUILD_SERVICE STREQUAL "C" OR BUILD_SERVICE STREQUAL "ALL")
  add_subdirectory(serviceC)
endif()

