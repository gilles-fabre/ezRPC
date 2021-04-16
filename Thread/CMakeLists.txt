cmake_minimum_required(VERSION 3.9)

project(ezThread)

if(${ROOT_DIR})
  set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}" CACHE PATH "set install root dir to : ${ROOT_DIR}" FORCE)
endif()

message("ezThread.ROOT_DIR = ${ROOT_DIR}")
message("ezThread.CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

if (WIN32)
add_library(ezThread STATIC Thread.cpp Thread.h)
else()
add_library(ezThread SHARED Thread.cpp Thread.h)
endif()

set_target_properties(ezThread PROPERTIES PUBLIC_HEADER Thread.h)

target_compile_definitions(ezThread PUBLIC EZTHREAD_LIBRARY_EXPORTS)

install(TARGETS ezThread
		PUBLIC_HEADER DESTINATION include
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
		