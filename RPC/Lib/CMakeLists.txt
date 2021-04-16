cmake_minimum_required(VERSION 3.9)

project(ezRPC)

if(${ROOT_DIR})
  set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}" CACHE PATH "set install root dir to : ${ROOT_DIR}" FORCE)
endif()

message("ezRPC.ROOT_DIR = ${ROOT_DIR}")
message("ezRPC.CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

if (WIN32)
add_library(ezRPC STATIC RemoteProcedureCall.cpp RemoteProcedureCall.h RPCServer.cpp RPCServer.h RPCClient.cpp RPCClient.h)
else()
add_library(ezRPC SHARED RemoteProcedureCall.cpp RemoteProcedureCall.h RPCServer.cpp RPCServer.h RPCClient.cpp RPCClient.h)
endif()

target_include_directories(ezRPC PUBLIC ${CMAKE_INSTALL_PREFIX}/include)

if (WIN32) 
	target_link_libraries(ezRPC ${CMAKE_INSTALL_PREFIX}/lib/ezThread.lib)
	target_link_libraries(ezRPC ${CMAKE_INSTALL_PREFIX}/lib/ezTransport.lib)
	target_link_libraries(ezRPC ${CMAKE_INSTALL_PREFIX}/lib/Log2Reporter.lib)
else()
	target_link_libraries(ezRPC ${CMAKE_INSTALL_PREFIX}/lib/ezThread.so)
	target_link_libraries(ezRPC ${CMAKE_INSTALL_PREFIX}/lib/ezTransport.so)
	target_link_libraries(ezRPC ${CMAKE_INSTALL_PREFIX}/lib/Log2Reporter.so)
endif()

target_compile_definitions(ezRPC PUBLIC EZRPC_LIBRARY_EXPORTS)

install(FILES RemoteProcedureCall.h RPCServer.h RPCClient.h DESTINATION include)

install(TARGETS ezRPC
		RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
		