cmake_minimum_required(VERSION 3.9)

project(ezRPC)

if(${ROOT_DIR})
  set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}" CACHE PATH "set install root dir to : ${ROOT_DIR}" FORCE)
else()
  set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "set install root dir to : ${CMAKE_SOURCE_DIR}" FORCE)
endif()

message("ezRPC.ROOT_DIR = ${ROOT_DIR}")
message("ezRPC.CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")


add_library(LogReporter SHARED 
			LogReporter/log2reporter.c)

add_library(ezRPC SHARED 
			RPC/RPCServer.cpp RPC/RPCClient.cpp RPC/RemoteProcedureCall.cpp
		    Transport/Transport.cpp Transport/Link.cpp
		    Thread/Thread.cpp)
						
target_link_libraries(ezRPC LogReporter)

add_executable(ezRPC_TestServer 
			   RPC/TestServer.cpp)

add_executable(ezRPC_TestClient 
			   RPC/TestClient.cpp)

target_link_libraries(ezRPC_TestServer ezRPC)
target_link_libraries(ezRPC_TestClient ezRPC)

if (NOT WIN32)
add_library(ezStateMachine SHARED
			StateMachine/StateMachine.cpp)

target_link_libraries(ezStateMachine LogReporter)

target_link_libraries(ezStateMachine ezRPC jsoncpp)

add_executable(ezStateMachine_Test 
			   StateMachine/test.cpp)

add_executable(ezStateMachineServer 
			   StateMachine/StateMachineServer.cpp)

target_link_libraries(ezStateMachine_Test ezStateMachine)

target_link_libraries(ezStateMachineServer ezStateMachine)
endif()

target_include_directories(ezRPC 
							PUBLIC
						   "${CMAKE_INSTALL_PREFIX}/RPC" 
						   "${CMAKE_INSTALL_PREFIX}/Transport" 
						   "${CMAKE_INSTALL_PREFIX}/Thread" 
						   "${CMAKE_INSTALL_PREFIX}/LogReporter")

target_include_directories(ezRPC_TestServer 
							PUBLIC
							"${CMAKE_INSTALL_PREFIX}/StateMachine" 
							"${CMAKE_INSTALL_PREFIX}/RPC" 
						    "${CMAKE_INSTALL_PREFIX}/Transport" 
						    "${CMAKE_INSTALL_PREFIX}/Thread" 
						    "${CMAKE_INSTALL_PREFIX}/LogReporter")

target_include_directories(ezRPC_TestClient
							PUBLIC
							"${CMAKE_INSTALL_PREFIX}/StateMachine" 
							"${CMAKE_INSTALL_PREFIX}/RPC" 
						    "${CMAKE_INSTALL_PREFIX}/Transport" 
						    "${CMAKE_INSTALL_PREFIX}/Thread" 
						    "${CMAKE_INSTALL_PREFIX}/LogReporter")

if (NOT WIN32)
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

endif()

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(ezRPC PUBLIC "-pthread")
  target_compile_options(ezRPC_TestServer PUBLIC "-pthread")
  target_compile_options(ezRPC_TestClient PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(ezRPC "${CMAKE_THREAD_LIBS_INIT}")
  target_link_libraries(ezRPC_TestServer "${CMAKE_THREAD_LIBS_INIT}")
  target_link_libraries(ezRPC_TestClient "${CMAKE_THREAD_LIBS_INIT}")
endif()

install(FILES RPC/ezRPC_TestServer.log-settings DESTINATION bin)
install(FILES RPC/ezRPC_TestClient.log-settings DESTINATION bin)

if (NOT WIN32)
install(FILES StateMachine/ezStateMachine_Test.log-settings DESTINATION bin)
install(FILES StateMachine/ezStateMachineServer.log-settings DESTINATION bin)
endif()

install(TARGETS LogReporter
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS ezRPC_TestServer RUNTIME DESTINATION bin)
install(TARGETS ezRPC_TestClient RUNTIME DESTINATION bin)

install(TARGETS ezRPC_TestServer
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS ezRPC_TestClient
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS ezRPC
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

if (NOT WIN32)
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
endif()
		
