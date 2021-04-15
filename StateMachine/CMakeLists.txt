cmake_minimum_required(VERSION 3.9)

project(ezStateMachine)

add_library(ezStateMachine SHARED StateMachine.cpp StateMachine.h)

target_include_directories(ezStateMachine PUBLIC ${PROJECT_SOURCE_DIR}/../LogReporter/log-2-reporter)
target_include_directories(ezStateMachine PUBLIC ${PROJECT_SOURCE_DIR}/../Thread)
target_include_directories(ezStateMachine PUBLIC ${PROJECT_SOURCE_DIR}/../Transport)
target_include_directories(ezStateMachine PUBLIC ${PROJECT_SOURCE_DIR}/../RPC)

set_target_properties(ezStateMachine PROPERTIES PUBLIC_HEADER StateMachine.h)

target_compile_definitions(ezStateMachine PUBLIC EZSM_LIBRARY_EXPORTS)

