/*
 * log2reporter.h
 *
 *  Created on: Oct 13, 2017
 *      Author: gilles-fabre
 */

#ifndef LOG2REPORTER_H_
#define LOG2REPORTER_H_


#ifdef WIN32
	#ifdef LOG2REPORTER_LIBRARY_EXPORTS
		#define LOG2REPORTER_LIBRARY_API __declspec(dllexport)
	#else
		#define LOG2REPORTER_LIBRARY_API __declspec(dllimport)
	#endif
#endif

#include <stdint.h>
#include <stdbool.h>

#define TRACE_REPORTER_DEFAULT_IP 		"127.0.0.1"
#define TRACE_REPORTER_DEFAULT_PORT		25998
#define MODID_LOG_MASK					"MODID_LOG_MASK"
#define MAX_LOG_SIZE_KEY				"MAX_LOG_SIZE"
#define SERVER_IP_KEY					"SERVER_IP"
#define SERVER_PORT_KEY					"SERVER_PORT"
#define AUTO_RECONNECT					"AUTO_RECONNECT"
#define TRACES_OFF						"TRACES_OFF"

/**
 * Log Settings File:
 *
 * you can create a <appname>.log-settings file in the program directory with the following
 * keys/values:
 *
 *	MAX_LOG_SIZE=S
 *	SERVER_IP=H
 *	SERVER_PORT=P
 *
 *	where:
 *		S is the maximum size of the log file for this program, in KBytes
 *		H is the IPv4 numeric address of the LogReporter
 *		P is the Port on which the LogReporter server is listening
 *
 */

#ifdef __cplusplus
extern "C" {
#endif
#ifdef WIN32
    LOG2REPORTER_LIBRARY_API void SetLogAddress(const char* ipAddress, uint16_t port);
    LOG2REPORTER_LIBRARY_API void LogVText(uint32_t module, uint8_t indent, bool lineBreak, const char* formatP, ...);
    LOG2REPORTER_LIBRARY_API void LogText(uint32_t module, uint8_t indent, bool lineBreak, const char* textP);
    LOG2REPORTER_LIBRARY_API void LogData(uint32_t module, uint8_t indent, const uint8_t* dataP, uint16_t size);
#else
    void SetLogAddress(const char* ipAddress, uint16_t port);
    void LogVText(uint32_t module, uint8_t indent, bool lineBreak, const char* formatP, ...);
    void LogText(uint32_t module, uint8_t indent, bool lineBreak, const char* textP);
    void LogData(uint32_t module, uint8_t indent, const uint8_t* dataP, uint16_t size);
#endif
#ifdef __cplusplus
};
#endif

#endif /* LOG2REPORTER_H_ */
