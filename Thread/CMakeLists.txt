cmake_minimum_required(VERSION 3.9)

project(ezThread)

add_library(ezThread SHARED Thread.cpp Thread.h)

set_target_properties(ezThread PROPERTIES PUBLIC_HEADER Thread.h)

target_compile_definitions(ezThread PUBLIC EZTHREAD_LIBRARY_EXPORTS)

