cmake_minimum_required(VERSION 3.9)

project(ezEClient)

add_library(ezEClient SHARED EClient.cpp EClient.h)

target_include_directories(ezEClient PUBLIC ${PROJECT_SOURCE_DIR}/../LogReporter/log-2-reporter)
target_include_directories(ezEClient PUBLIC ${PROJECT_SOURCE_DIR}/../Thread)
target_include_directories(ezEClient PUBLIC ${PROJECT_SOURCE_DIR}/../Transport)
target_include_directories(ezEClient PUBLIC ${PROJECT_SOURCE_DIR}/../RPC)

set_target_properties(ezEClient PROPERTIES PUBLIC_HEADER EClient.h)

target_compile_definitions(ezEClient PUBLIC EZECLIENT_LIBRARY_EXPORTS)

