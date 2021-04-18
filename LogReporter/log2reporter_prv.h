/*
 * log2reporter_prv.h
 *
 *  Created on: Oct 13, 2017
 *      Author: gilles-fabre
 */

#ifndef LOG2REPORTER_PRV_H_
#define LOG2REPORTER_PRV_H_

#include <stdint.h>

#define PROTOCOL_VERSION_MAJOR 			1
#define PROTOCOL_VERSION_MINOR 			1

#define LOG_FILE_EXTENSION 				".log"
#define LOG_SETTINGS_EXTENSION 			".log-settings"

#define START_DUMP_MSG 		">>> START DUMP"
#define END_DUMP_MSG 		"<<< END DUMP"

typedef enum ParameterType {
	EOM,
	ATTR,
	INDENT,
	PID,
	TID,
	DATA,
	TIME,
	APPNAME
} ParameterType;

typedef enum DataType {
	SINGLELINE,
	MULTILINE,
	BINARY,
	START_DUMP_TAG,
	END_DUMP_TAG
} DataType;

typedef uint8_t 	INDENT_DATA_TYPE;
typedef uint32_t 	PID_DATA_TYPE;
typedef uint32_t 	TID_DATA_TYPE;
typedef uint64_t 	TIME_DATA_TYPE;
typedef uint32_t 	MODULE_DATA_TYPE;
typedef uint8_t 	TYPE_DATA_TYPE;

#define INDENT_DATA_SIZE 	sizeof(INDENT_DATA_TYPE)
#define PID_DATA_SIZE 		sizeof(PID_DATA_TYPE)
#define TID_DATA_SIZE 		sizeof(TID_DATA_TYPE)
#define TIME_DATA_SIZE 		sizeof(TIME_DATA_TYPE)
#define ATTR_DATA_SIZE 		(sizeof(MODULE_DATA_TYPE) + sizeof(TYPE_DATA_TYPE))
// NOTE: APPNAME and DATA data have variable length

typedef struct {
	uint8_t 	type;				// parameter type
	uint16_t 	size;				// parameter data (only) size
} ParameterHeader;
//#define PARAM_HEADER_SIZE (sizeof(ParameterHeader.type) + sizeof(ParameterHeader.size))
#define PARAM_HEADER_SIZE (sizeof(uint8_t) + sizeof(uint16_t))

typedef struct {
	ParameterHeader		header;		// parameter header
	uint8_t			*dataP;		// header.size bytes - PARAM_HEADER_SIZE
} Parameter;

typedef struct {
	uint16_t version; 				// protocol version
	uint32_t size;					// message size, including header
} MessageHeader;
//#define MESSAGE_HEADER_SIZE (sizeof(MessageHeader.version) + sizeof(MessageHeader.size))
#define MESSAGE_HEADER_SIZE (sizeof(uint16_t) + sizeof(uint32_t))

enum MessageParam {
	APPNAME_PARAM,
	ATTR_PARAM,
	INDENT_PARAM,
	PID_PARAM,
	TID_PARAM,
	DATA_PARAM,
	TIME_PARAM,
	EOM_PARAM,
	NUM_PARAMS
};

typedef struct {
	MessageHeader 	header;					// message header
	Parameter   	parameters[NUM_PARAMS];	// parameters
} Message;

#endif /* LOG2REPORTER_PRV_H_ */
