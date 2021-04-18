cmake_minimum_required(VERSION 3.9)

project(ezRPC)

if(${ROOT_DIR})
  set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}" CACHE PATH "set install root dir to : ${ROOT_DIR}" FORCE)
else()
  set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "set install root dir to : ${CMAKE_SOURCE_DIR}" FORCE)
endif()

message("ezRPC.ROOT_DIR = ${ROOT_DIR}")
message("ezRPC.CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

add_executable(ezRPC_Test 
			   RPC/test.cpp 
			   RPC/RPCServer.cpp RPC/RPCClient.cpp RPC/RemoteProcedureCall.cpp
			   Transport/Transport.cpp Transport/Link.cpp
			   Thread/Thread.cpp
			   LogReporter/log2reporter.c)
			   
add_library(ezRPC SHARED 
			RPC/RPCServer.cpp RPC/RPCClient.cpp RPC/RemoteProcedureCall.cpp
		    Transport/Transport.cpp Transport/Link.cpp
		    Thread/Thread.cpp
		    LogReporter/log2reporter.c)

target_include_directories(ezRPC 
							PUBLIC
						   "${CMAKE_INSTALL_PREFIX}/RPC" 
						   "${CMAKE_INSTALL_PREFIX}/Transport" 
						   "${CMAKE_INSTALL_PREFIX}/Thread" 
						   "${CMAKE_INSTALL_PREFIX}/LogReporter")

target_include_directories(ezRPC_Test 
							PUBLIC
						   "${CMAKE_INSTALL_PREFIX}/RPC" 
						   "${CMAKE_INSTALL_PREFIX}/Transport" 
						   "${CMAKE_INSTALL_PREFIX}/Thread" 
						   "${CMAKE_INSTALL_PREFIX}/LogReporter")

install(FILES RPC/ezRPC_Test.log-settings DESTINATION bin)
install(TARGETS ezRPC_Test RUNTIME DESTINATION bin)
install(TARGETS ezRPC_Test
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS ezRPC
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(FILES 
		RPC/RPCClient.h
		RPC/RPCServer.h
		RPC/RemoteProcedureCall.h
		DESTINATION include)
		