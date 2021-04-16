cmake_minimum_required(VERSION 3.9)

project(ezRPC_Test)

if(${ROOT_DIR})
  set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}" CACHE PATH "set install root dir to : ${ROOT_DIR}" FORCE)
endif()

message("ezRPC_Test.ROOT_DIR = ${ROOT_DIR}")
message("ezRPC_Test.CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

add_executable(ezRPC_Test test.cpp)

target_include_directories(ezRPC_Test PRIVATE ${CMAKE_INSTALL_PREFIX}/include)

if (WIN32) 
	target_link_libraries(ezRPC_Test ${CMAKE_INSTALL_PREFIX}/lib/ezThread.lib)
	target_link_libraries(ezRPC_Test ${CMAKE_INSTALL_PREFIX}/lib/ezTransport.lib)
	target_link_libraries(ezRPC_Test ${CMAKE_INSTALL_PREFIX}/lib/ezRPC.lib)
	target_link_libraries(ezRPC_Test ${CMAKE_INSTALL_PREFIX}/lib/Log2Reporter.lib)
else()
	target_link_libraries(ezRPC_Test ${CMAKE_INSTALL_PREFIX}/lib/libezThread.so)
	target_link_libraries(ezRPC_Test ${CMAKE_INSTALL_PREFIX}/lib/libezTransport.so)
	target_link_libraries(ezRPC_Test ${CMAKE_INSTALL_PREFIX}/lib/libezRPC.so)
	target_link_libraries(ezRPC_Test ${CMAKE_INSTALL_PREFIX}/lib/libLog2Reporter.so)
endif()

install(FILES ezRPC_Test.log-settings DESTINATION bin)
install(TARGETS ezRPC_Test
        RUNTIME DESTINATION bin)
		