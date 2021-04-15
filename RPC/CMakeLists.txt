cmake_minimum_required(VERSION 3.9)

project(ezRPC)
project(ezRPC_Test)

add_library(ezRPC SHARED RemoteProcedureCall.cpp RemoteProcedureCall.h RPCServer.cpp RPCServer.h RPCClient.cpp RPCClient.h)

add_executable(ezRPC_Test test.cpp)

target_include_directories(ezRPC_Test PUBLIC ${PROJECT_SOURCE_DIR}/../LogReporter/log-2-reporter)
target_include_directories(ezRPC_Test PUBLIC ${PROJECT_SOURCE_DIR}/../Thread)
target_include_directories(ezRPC_Test PUBLIC ${PROJECT_SOURCE_DIR}/../Transport)

target_include_directories(ezRPC PUBLIC ${PROJECT_SOURCE_DIR}/../LogReporter/log-2-reporter)
target_include_directories(ezRPC PUBLIC ${PROJECT_SOURCE_DIR}/../Thread)
target_include_directories(ezRPC PUBLIC ${PROJECT_SOURCE_DIR}/../Transport)

target_link_libraries(ezRPC_Test libpthread.so)
target_link_libraries(ezRPC_Test ${PROJECT_SOURCE_DIR}/../Thread/build/libezThread.so)
target_link_libraries(ezRPC_Test ${PROJECT_SOURCE_DIR}/../Transport/build/libezTransport.so)
target_link_libraries(ezRPC_Test ${PROJECT_SOURCE_DIR}/build/libezRPC.so)
target_link_libraries(ezRPC_Test ${PROJECT_SOURCE_DIR}/../LogReporter/log-2-reporter/build/libLog2Reporter.so)

set_target_properties(ezRPC PROPERTIES PUBLIC_HEADER RemoteProcedureCall.h)
set_target_properties(ezRPC PROPERTIES PUBLIC_HEADER RPCServer.h)
set_target_properties(ezRPC PROPERTIES PUBLIC_HEADER RPCClient.h)

target_compile_definitions(ezRPC PUBLIC EZRPC_LIBRARY_EXPORTS)

