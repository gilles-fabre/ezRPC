cmake_minimum_required(VERSION 3.9)

project(Log2Reporter)

if(${ROOT_DIR})
  set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}" CACHE PATH "set install root dir to : ${ROOT_DIR}" FORCE)
endif()

message("Log2Reporter.ROOT_DIR = ${ROOT_DIR}")
message("Log2Reporter.CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

add_library(Log2Reporter SHARED log2reporter.c log2reporter.h log2reporter_prv.h)

set_target_properties(Log2Reporter PROPERTIES PUBLIC_HEADER log2reporter.h)

target_compile_definitions(Log2Reporter PUBLIC LOG2REPORTER_LIBRARY_EXPORTS)

install(TARGETS Log2Reporter
		PUBLIC_HEADER DESTINATION include
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
		