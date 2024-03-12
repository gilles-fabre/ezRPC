cmake_minimum_required(VERSION 3.9)

project(ezRPC)
if (NOT WIN32)
set(CMAKE_CXX_STANDARD 17) 
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

if (NOT WIN32)
set(CMAKE_BUILD_TYPE "Debug")  # remove this to generate Release makefiles under Linux
endif()

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
			RPC/JsonRPCClient.cpp RPC/JsonRPCServer.cpp RPC/RPCServer.cpp RPC/RPCClient.cpp RPC/RemoteProcedureCall.cpp RPC/JsonParameters.cpp
		    Transport/Transport.cpp Transport/TcpTransport.cpp Transport/FileTransport.cpp Transport/Link.cpp
		    Thread/Thread.cpp)

target_link_libraries(ezRPC LogReporter)

add_executable(ezRPC_TestServer 
			   RPC/TestServer.cpp)

add_executable(ezRPC_TestClient 
			   RPC/TestClient.cpp)

add_executable(ezRPC_JsonTestClient 
			   RPC/JsonTestClient.cpp)

add_executable(ezRPC_JsonTestServer
			   RPC/JsonTestServer.cpp)

target_link_libraries(ezRPC_TestServer ezRPC LogReporter)
target_link_libraries(ezRPC_TestClient ezRPC LogReporter)
target_link_libraries(ezRPC_JsonTestClient ezRPC LogReporter)
target_link_libraries(ezRPC_JsonTestServer ezRPC LogReporter)

target_include_directories(ezRPC 
							PUBLIC
						   "${CMAKE_INSTALL_PREFIX}/ReturnValue" 
						   "${CMAKE_INSTALL_PREFIX}/RPC/json/include" 
						   "${CMAKE_INSTALL_PREFIX}/RPC" 
						   "${CMAKE_INSTALL_PREFIX}/Transport" 
						   "${CMAKE_INSTALL_PREFIX}/Thread" 
						   "${CMAKE_INSTALL_PREFIX}/LogReporter")

target_include_directories(ezRPC_TestServer 
							PUBLIC
						   "${CMAKE_INSTALL_PREFIX}/ReturnValue" 
							"${CMAKE_INSTALL_PREFIX}/RPC" 
						    "${CMAKE_INSTALL_PREFIX}/Transport" 
						    "${CMAKE_INSTALL_PREFIX}/Thread" 
						    "${CMAKE_INSTALL_PREFIX}/LogReporter")

target_include_directories(ezRPC_TestClient
							PUBLIC
						   "${CMAKE_INSTALL_PREFIX}/ReturnValue" 
							"${CMAKE_INSTALL_PREFIX}/RPC" 
						    "${CMAKE_INSTALL_PREFIX}/Transport" 
						    "${CMAKE_INSTALL_PREFIX}/Thread" 
						    "${CMAKE_INSTALL_PREFIX}/LogReporter")

target_include_directories(ezRPC_JsonTestClient
							PUBLIC
						   "${CMAKE_INSTALL_PREFIX}/ReturnValue" 
						   "${CMAKE_INSTALL_PREFIX}/RPC/json/include" 
							"${CMAKE_INSTALL_PREFIX}/RPC" 
						    "${CMAKE_INSTALL_PREFIX}/Transport" 
						    "${CMAKE_INSTALL_PREFIX}/Thread" 
						    "${CMAKE_INSTALL_PREFIX}/LogReporter")

target_include_directories(ezRPC_JsonTestServer
							PUBLIC
						   "${CMAKE_INSTALL_PREFIX}/ReturnValue" 
						   "${CMAKE_INSTALL_PREFIX}/RPC/json/include" 
							"${CMAKE_INSTALL_PREFIX}/RPC" 
						    "${CMAKE_INSTALL_PREFIX}/Transport" 
						    "${CMAKE_INSTALL_PREFIX}/Thread" 
						    "${CMAKE_INSTALL_PREFIX}/LogReporter")

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(ezRPC PUBLIC "-pthread")
  target_compile_options(ezRPC_TestServer PUBLIC "-pthread")
  target_compile_options(ezRPC_TestClient PUBLIC "-pthread")
  target_compile_options(ezRPC_JsonTestClient PUBLIC "-pthread")
  target_compile_options(ezRPC_JsonTestServer PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(ezRPC "${CMAKE_THREAD_LIBS_INIT}")
  target_link_libraries(ezRPC_TestServer "${CMAKE_THREAD_LIBS_INIT}")
  target_link_libraries(ezRPC_TestClient "${CMAKE_THREAD_LIBS_INIT}")
  target_link_libraries(ezRPC_JsonTestClient "${CMAKE_THREAD_LIBS_INIT}")
  target_link_libraries(ezRPC_JsonTestServer "${CMAKE_THREAD_LIBS_INIT}")
endif()

install(FILES RPC/ezRPC_TestServer.log-settings DESTINATION bin)
install(FILES RPC/ezRPC_TestClient.log-settings DESTINATION bin)
install(FILES RPC/ezRPC_JsonTestServer.log-settings DESTINATION bin)
install(FILES RPC/ezRPC_JsonTestClient.log-settings DESTINATION bin)

install(TARGETS LogReporter
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS ezRPC_TestServer RUNTIME DESTINATION bin)
install(TARGETS ezRPC_TestClient RUNTIME DESTINATION bin)
install(TARGETS ezRPC_JsonTestClient RUNTIME DESTINATION bin)
install(TARGETS ezRPC_JsonTestServer RUNTIME DESTINATION bin)

install(TARGETS ezRPC_TestServer
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS ezRPC_TestClient
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS ezRPC_JsonTestClient
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS ezRPC_JsonTestServer
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS ezRPC
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

		
