cmake_minimum_required(VERSION 3.9)

project(ezTransport)

if(${ROOT_DIR})
  set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}" CACHE PATH "set install root dir to : ${ROOT_DIR}" FORCE)
endif()

message("ezTransport.ROOT_DIR = ${ROOT_DIR}")
message("ezTransport.CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

if (WIN32)
add_library(ezTransport STATIC Link.cpp Link.h Transport.cpp Transport.h)
else()
add_library(ezTransport SHARED Link.cpp Link.h Transport.cpp Transport.h)
endif()

target_include_directories(ezTransport PUBLIC ${CMAKE_INSTALL_PREFIX}/include)

if (WIN32)
target_link_libraries(ezTransport ${CMAKE_INSTALL_PREFIX}/lib/log2reporter.lib)
else()
target_link_libraries(ezTransport ${CMAKE_INSTALL_PREFIX}/lib/log2reporter.so)
endif()

target_compile_definitions(ezTransport PUBLIC EZTRANSPORT_LIBRARY_EXPORTS)

install(FILES Link.h Transport.h DESTINATION include)

install(TARGETS ezTransport
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
		