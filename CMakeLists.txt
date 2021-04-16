cmake_minimum_required(VERSION 3.9)

project(ezRPC)

set(ROOT_DIR ${CMAKE_SOURCE_DIR} CACHE INTERNAL "")

message("ezRPC.ROOT_DIR = ${ROOT_DIR}")

add_subdirectory(LogReporter)
add_subdirectory(Thread)
add_subdirectory(Transport)
add_subdirectory(RPC)
