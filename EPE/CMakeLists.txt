cmake_minimum_required(VERSION 3.9)

project(ezEPE)
project(ezEPE_Test)

add_executable(ezEPE_Test test.cpp)
add_executable(ezEPE test.cpp)

target_include_directories(ezEPE_Test PUBLIC ${PROJECT_SOURCE_DIR}/../LogReporter/log-2-reporter)
target_include_directories(ezEPE_Test PUBLIC ${PROJECT_SOURCE_DIR}/../Thread)
target_include_directories(ezEPE_Test PUBLIC ${PROJECT_SOURCE_DIR}/../Transport)
target_include_directories(ezEPE_Test PUBLIC ${PROJECT_SOURCE_DIR}/../RPC)
target_include_directories(ezEPE_Test PUBLIC ${PROJECT_SOURCE_DIR}/../StateMachine)
target_include_directories(ezEPE_Test PUBLIC ${PROJECT_SOURCE_DIR}/../EClient)

target_include_directories(ezEPE PUBLIC ${PROJECT_SOURCE_DIR}/../LogReporter/log-2-reporter)
target_include_directories(ezEPE PUBLIC ${PROJECT_SOURCE_DIR}/../Thread)
target_include_directories(ezEPE PUBLIC ${PROJECT_SOURCE_DIR}/../Transport)
target_include_directories(ezEPE PUBLIC ${PROJECT_SOURCE_DIR}/../RPC)
target_include_directories(ezEPE PUBLIC ${PROJECT_SOURCE_DIR}/../StateMachine)
target_include_directories(ezEPE PUBLIC ${PROJECT_SOURCE_DIR}/../EClient)

target_link_libraries(ezEPE_Test libpthread.so)

target_link_libraries(ezEPE_Test ${PROJECT_SOURCE_DIR}/../Thread/build/libezThread.so)
target_link_libraries(ezEPE_Test ${PROJECT_SOURCE_DIR}/../Transport/build/libezTransport.so)
target_link_libraries(ezEPE_Test ${PROJECT_SOURCE_DIR}/../RPC/build/libezRPC.so)
target_link_libraries(ezEPE_Test ${PROJECT_SOURCE_DIR}/../StateMachine/build/libezStateMachine.so)
target_link_libraries(ezEPE_Test ${PROJECT_SOURCE_DIR}/../EClient/build/libezEClient.so)
target_link_libraries(ezEPE_Test ${PROJECT_SOURCE_DIR}/../LogReporter/log-2-reporter/build/libLog2Reporter.so)

target_link_libraries(ezEPE libpthread.so)

target_link_libraries(ezEPE ${PROJECT_SOURCE_DIR}/../Thread/build/libezThread.so)
target_link_libraries(ezEPE ${PROJECT_SOURCE_DIR}/../Transport/build/libezTransport.so)
target_link_libraries(ezEPE ${PROJECT_SOURCE_DIR}/../RPC/build/libezRPC.so)
target_link_libraries(ezEPE ${PROJECT_SOURCE_DIR}/../StateMachine/build/libezStateMachine.so)
target_link_libraries(ezEPE ${PROJECT_SOURCE_DIR}/../EClient/build/libezEClient.so)
target_link_libraries(ezEPE ${PROJECT_SOURCE_DIR}/../LogReporter/log-2-reporter/build/libLog2Reporter.so)

