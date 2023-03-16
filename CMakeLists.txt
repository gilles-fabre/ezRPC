cmake_minimum_required(VERSION 3.9)

project(ezRPC)

if(${ROOT_DIR})
  set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}" CACHE PATH "set install root dir to : ${ROOT_DIR}" FORCE)
else()
  set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "set install root dir to : ${CMAKE_SOURCE_DIR}" FORCE)
endif()

message("ezRPC.ROOT_DIR = ${ROOT_DIR}")
message("ezRPC.CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")


add_library(ezRPC SHARED 
			RPC/RPCServer.cpp RPC/RPCClient.cpp RPC/RemoteProcedureCall.cpp
		    Transport/Transport.cpp Transport/Link.cpp
		    Thread/Thread.cpp
		    LogReporter/log2reporter.c)
						
add_executable(ezRPC_Test 
			   RPC/test.cpp)

target_link_libraries(ezRPC_Test ezRPC)

add_library(ezStateMachine SHARED
			StateMachine/StateMachine.cpp
		    LogReporter/log2reporter.c)

add_executable(ezStateMachine_Test 
			   StateMachine/test.cpp)

add_executable(ezStateMachineServer 
			   StateMachine/StateMachineServer.cpp)

target_link_libraries(ezStateMachine_Test ezStateMachine)

target_link_libraries(ezStateMachineServer ezStateMachine)

target_link_libraries(ezStateMachine ezRPC jsoncpp)

target_include_directories(ezRPC 
							PUBLIC
						   "${CMAKE_INSTALL_PREFIX}/RPC" 
						   "${CMAKE_INSTALL_PREFIX}/Transport" 
						   "${CMAKE_INSTALL_PREFIX}/Thread" 
						   "${CMAKE_INSTALL_PREFIX}/LogReporter")

target_include_directories(ezRPC_Test 
							PUBLIC
							"${CMAKE_INSTALL_PREFIX}/StateMachine" 
							"${CMAKE_INSTALL_PREFIX}/RPC" 
						    "${CMAKE_INSTALL_PREFIX}/Transport" 
						    "${CMAKE_INSTALL_PREFIX}/Thread" 
						    "${CMAKE_INSTALL_PREFIX}/LogReporter")

target_include_directories(ezStateMachine 
						   PUBLIC
						  "${CMAKE_INSTALL_PREFIX}/RPC" 
						  "${CMAKE_INSTALL_PREFIX}/Transport" 
						  "${CMAKE_INSTALL_PREFIX}/Thread" 
						  "${CMAKE_INSTALL_PREFIX}/LogReporter")

target_include_directories(ezStateMachine_Test 
							PUBLIC
							"${CMAKE_INSTALL_PREFIX}/StateMachine" 
							"${CMAKE_INSTALL_PREFIX}/RPC" 
							"${CMAKE_INSTALL_PREFIX}/Transport" 
							"${CMAKE_INSTALL_PREFIX}/Thread" 
							"${CMAKE_INSTALL_PREFIX}/LogReporter")
							
target_include_directories(ezStateMachineServer
							PUBLIC
							"${CMAKE_INSTALL_PREFIX}/StateMachine" 
							"${CMAKE_INSTALL_PREFIX}/RPC" 
							"${CMAKE_INSTALL_PREFIX}/Transport" 
							"${CMAKE_INSTALL_PREFIX}/Thread" 
							"${CMAKE_INSTALL_PREFIX}/LogReporter")


find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(ezRPC PUBLIC "-pthread")
  target_compile_options(ezRPC_Test PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(ezRPC "${CMAKE_THREAD_LIBS_INIT}")
  target_link_libraries(ezRPC_Test "${CMAKE_THREAD_LIBS_INIT}")
endif()

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

install(TARGETS ezStateMachine
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS ezStateMachineServer
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS ezStateMachine_Test
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(FILES 
		StateMachine/StateMachine.h
		RPC/RPCClient.h
		RPC/RPCServer.h
		RPC/RemoteProcedureCall.h
		DESTINATION include)
		
