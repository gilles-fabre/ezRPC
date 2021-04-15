cmake_minimum_required(VERSION 3.9)

project(Log2Reporter_Test)

add_executable(Log2Reporter_Test main.c)

target_include_directories(Log2Reporter_Test PRIVATE ${PROJECT_SOURCE_DIR}/../log-2-reporter)

if(${WIN32}) 
	target_link_libraries(Log2Reporter_Test ${PROJECT_SOURCE_DIR}/../log-2-reporter/build/Debug/log2reporter.lib)
else() 
	target_link_libraries(Log2Reporter_Test ${PROJECT_SOURCE_DIR}/../log-2-reporter/build/libLog2Reporter.so)
endif()

target_compile_definitions(Log2Reporter_Test PUBLIC)
