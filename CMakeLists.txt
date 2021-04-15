cmake_minimum_required(VERSION 3.9)

project(ezRPC)

add_subdirectory(LogReporter/log-2-reporter)
add_subdirectory(LogReporter/log-reporter-test)
add_subdirectory(Thread)
add_subdirectory(Transport)
add_subdirectory(RPC)
add_subdirectory(StateMachine)
add_subdirectory(EClient)
add_subdirectory(EPE)
