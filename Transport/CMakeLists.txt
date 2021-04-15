cmake_minimum_required(VERSION 3.9)

project(ezTransport)

add_library(ezTransport SHARED Link.cpp Link.h Transport.cpp Transport.h)

target_include_directories(ezTransport PUBLIC ${PROJECT_SOURCE_DIR}/../LogReporter/log-2-reporter)

set_target_properties(ezTransport PROPERTIES PUBLIC_HEADER Link.h)
set_target_properties(ezTransport PROPERTIES PUBLIC_HEADER Transport.h)

target_compile_definitions(ezTransport PUBLIC EZTRANSPORT_LIBRARY_EXPORTS)

