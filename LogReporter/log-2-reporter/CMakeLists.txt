cmake_minimum_required(VERSION 3.9)

project(Log2Reporter)

add_library(Log2Reporter SHARED log2reporter.c log2reporter.h log2reporter_prv.h)

set_target_properties(Log2Reporter PROPERTIES PUBLIC_HEADER log2reporter.h)

target_compile_definitions(Log2Reporter PRIVATE LOG2REPORTER_LIBRARY_EXPORTS)

