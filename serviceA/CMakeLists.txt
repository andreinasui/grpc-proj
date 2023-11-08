set(EXECUTABLE_NAME service_a)
set(SOURCES main.cc)

add_executable(${EXECUTABLE_NAME} ${SOURCES})

target_link_libraries(${EXECUTABLE_NAME} grpc_proto)
