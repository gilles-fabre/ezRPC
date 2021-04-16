cmake_minimum_required(VERSION 3.9)

project(Log2Reporter_Test)

if(${ROOT_DIR})
  set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}" CACHE PATH "set install root dir to : ${ROOT_DIR}" FORCE)
endif()

message("Log2Reporter_Test.ROOT_DIR = ${ROOT_DIR}")
message("Log2Reporter_Test.CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

add_executable(Log2Reporter_Test main.c)

target_include_directories(Log2Reporter_Test PRIVATE ${CMAKE_INSTALL_PREFIX}/include)

if (WIN32)
target_link_libraries(Log2Reporter_Test ${CMAKE_INSTALL_PREFIX}/lib/log2reporter.lib)
else()
target_link_libraries(Log2Reporter_Test ${CMAKE_INSTALL_PREFIX}/lib/log2reporter.so)
endif()

target_compile_definitions(Log2Reporter_Test PUBLIC)

install(FILES Log2Reporter_Test.log-settings DESTINATION bin)
install(TARGETS Log2Reporter_Test RUNTIME DESTINATION bin)