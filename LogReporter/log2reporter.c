/**
 * \file log2reporter.c
 * \brief shared library used to communicate with log-reporter (LogReporter.jar)
 *
 * \data Oct 13, 2017
 * \author: gilles-fabre


 */


#ifdef WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS  1

// windows headers
#include <ws2tcpip.h> 
#include <fcntl.h>
#include <sys/stat.h>
#include <process.h>
#include <io.h>
#include <sys/timeb.h>

#ifndef LOG2REPORTER_LIBRARY_EXPORTS
	#define LOG2REPORTER_LIBRARY_EXPORTS
#endif

#else

// linux headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/syscall.h>
#include <sys/file.h>
#include <unistd.h>
#include <netdb.h>
#include <error.h>
#include <sys/time.h>

#endif

// cross-platform headers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>

#include "log2reporter_prv.h"

#include "log2reporter.h"

int 		gMaxLogSize = 0;
int	 		gLogSocket = -1;
int         gLogFile = -1;
bool		gSettingsLoaded = false;
char 		gServerIpAddress[15 + 1] = TRACE_REPORTER_DEFAULT_IP;
uint16_t 	gServerPort = TRACE_REPORTER_DEFAULT_PORT;
uint32_t 	gModIdLogMask = 0xFFFFFFFF;
#ifdef WIN32
char 		gProcessName[MAX_PATH  + 1] = "";
#else
char 		gProcessName[PATH_MAX  + 1] = "";
#endif

 
#ifdef WIN32
bool		gWSAStartupDone = false;

#endif
 
bool        gTracesOff = true;			// don't trace at all
bool        gAutoReconnect = false;		// will try to reconnect every single message
bool        gConnectionFailed = false;      	// last connection failed, will retry if gAutoReconnect is set

/**
 * \fn static const char *GetProcessName()
 * \brief return the name of the current process
 *
 */
static const char *GetProcessName() {
	if (*gProcessName)
		return gProcessName;

#ifdef WIN32
	GetModuleFileNameA(NULL, gProcessName, sizeof(gProcessName));
	_splitpath_s(gProcessName, 
				 NULL, 0,
				 NULL, 0,
				 gProcessName, sizeof(gProcessName) - 1,
				 NULL, 0);
#else
	FILE *f = fopen("/proc/self/cmdline", "r");
	if (f) {
		size_t size;
		if ((size = fread(gProcessName, sizeof(char), PATH_MAX, f)) > 0) {
			gProcessName[size] = '\0';

			// cleanup, just want the process name, not the arguments
			for (int i = 0; gProcessName[i]; i++)
				if (gProcessName[i] == '\t' || gProcessName[i] == ' ') {
					gProcessName[i] = '\0';
					break;
				}
		}
		fclose(f);
	}


#endif
	return gProcessName;
}

/**
 * \fn static void InitSettings(const char *appName)
 * \brief Load the log settings from the LOG_SETTINGS_FILE (once per client process). This is
 *        not protected against concurrent access to the settings file.
 *
 * \param appName is the name of the calling application
 *
 */
static char *Trim(char *stringP) {
	if (stringP) {
		int i = 0, j = 0;
		while (stringP[i] != '\0') {
			if (stringP[i] != ' ' && stringP[i] != '\t')
				stringP[j++] = stringP[i];
			i++;
		}
		stringP[j] = '\0';
	}
	return stringP;
}
static void InitSettings(const char *appName) {
	if (gSettingsLoaded)
		return;

	// we're done with this
	gSettingsLoaded = true;

	// tries to open the settings file
#ifdef WIN32
	char logSettings[MAX_PATH + 1];
#else
	char logSettings[PATH_MAX + 1];
#endif
	snprintf(logSettings, sizeof(logSettings), "%s%s", appName, LOG_SETTINGS_EXTENSION);

	FILE* file = NULL;
#ifdef WIN32
	fopen_s(&file, logSettings, "r");
	if (!file) {
		char errBuffer[256 + 1];
		strerror_s(errBuffer, sizeof(errBuffer), errno);
		fprintf(stderr, "couldn't open setting file %s: %s\n", logSettings, errBuffer);
#else
	if (!(file = fopen(logSettings, "r"))) {
		fprintf(stderr, "couldn't open setting file %s: %s\n", logSettings, strerror(errno));

#endif
		return;
	}

	// iterates on lines
	char line[256 + 1];
	char *dummy;
	while (fgets(line, sizeof(line), file)) {
		Trim(line);
		// read the setting
		printf("settings line : %s\n", line);
#ifdef WIN32		
		char* nextP = NULL;
		char* keyP = strtok_s(line, "=", &nextP);
		char* valueP = keyP ? strtok_s(NULL, "=", &nextP) : NULL;
		if (keyP && valueP) {

#else
		char *keyP;
		char *valueP;
		if ((keyP = strtok(line, "=")) &&
			(valueP = strtok(NULL, "="))) {
#endif
			if (!strcmp(keyP, MAX_LOG_SIZE_KEY)) {
				printf("set max log size to : %s KBytes\n", valueP);
				gMaxLogSize = atoi(valueP);
			} else if (!strcmp(keyP, SERVER_IP_KEY)) {
				printf("set log address to : %s\n", valueP);
#ifdef WIN32
				strncpy_s(gServerIpAddress, sizeof(gServerIpAddress), valueP, strlen(valueP));
#else

				strncpy(gServerIpAddress, valueP, sizeof(gServerIpAddress));
#endif
				gServerIpAddress[sizeof(gServerIpAddress) - 1] = '\0';
			} else if (!strcmp(keyP, SERVER_PORT_KEY)) {
				printf("set log port to : %s\n", valueP);
#ifdef WIN32
				gServerPort  = (uint16_t)atoi(valueP);
#else
				gServerPort  = atoi(valueP);
#endif
			} else if (!strcmp(keyP, MODID_LOG_MASK)) {
				printf("set modid log module to : %s\n", valueP);
				gModIdLogMask  = strtoul(valueP, &dummy, 0);
			} else if (!strcmp(keyP, AUTO_RECONNECT)) {
				printf("set auto reconnect to : %s\n", valueP);
				gAutoReconnect  = (bool)atoi(valueP);
			} else if (!strcmp(keyP, TRACES_OFF)) {
				printf("set traces-off to : %s\n", valueP);
				gTracesOff  = (bool)atoi(valueP);
			}
		}
	}

	// we're done with the file
	fclose(file);
}

/**
 * \fn tic inline TIME_DATA_TYPE GetMilliTime(void)
 * \brief returns the epoch time in milliseconds
 */
#ifdef WIN32
static inline TIME_DATA_TYPE GetMilliTime(void) {
	struct timeb tb;
	ftime(&tb);
	return (tb.time * 1000L) + tb.millitm;
}
#else
static inline uint64_t GetMilliTime() {
	struct timeval time;
	gettimeofday(&time, NULL);
	return ((uint64_t)(time.tv_sec) * 1000) + (time.tv_usec / 1000);
}
#endif

/**
 * \fn static void BuildMessage(Message *messageP, const char *appName, uint32_t module, uint8_t indent, const uint8_t *dataP, uint16_t size, TYPE_DATA_TYPE type)
 * \brief build a protocol log message and send it to the connected log-reporter server
 *
 * \param messageP points to the message to be formatted
 * \param appName is the either the fully qualified or base name of the executable issuing the call. Can be used to filter in/colorize
 *        the log message
 * \param module is the module bit mask to be used to filter in/colorise the log message
 * \param indent is the indentation level for the log message
 * \param dataP points to the log message data, can be either a C string or a binary array depending on the data type
 * \param type specifies the type of data pointed by dataP
 * \param size is the length of the binary array referenced by dataP
 */
#ifndef WIN32
#define htonll(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#endif

static void BuildMessage(Message *messageP, const char *appName, uint32_t module, uint8_t indent, const uint8_t *dataP, uint16_t size, TYPE_DATA_TYPE type) {
	// get pid and tid.
	uint32_t pid, tid;
#ifdef WIN32
	pid = (uint32_t)_getpid();
	tid = (uint32_t)GetCurrentThreadId();

#else	
	pid = (uint32_t)getpid();
	tid = (uint32_t)syscall(SYS_gettid);

#endif
	// build message structure
	// app name parameter
	Parameter *appNameParamP = &messageP->parameters[APPNAME_PARAM];
	appNameParamP->header.size = (uint16_t)strlen(appName) + 1;
	appNameParamP->dataP = (uint8_t *)malloc(appNameParamP->header.size);
	assert(appNameParamP->dataP != NULL);
	memcpy(appNameParamP->dataP, appName, appNameParamP->header.size);
	appNameParamP->header.type = APPNAME;

	// attributes
	Parameter *attrParamP = &messageP->parameters[ATTR_PARAM];
	attrParamP->header.size = ATTR_DATA_SIZE;
	attrParamP->dataP = (uint8_t*)malloc(ATTR_DATA_SIZE);
	assert(attrParamP->dataP != NULL);
	*(MODULE_DATA_TYPE *)(attrParamP->dataP) = (MODULE_DATA_TYPE)htonl(module);
	*(TYPE_DATA_TYPE *)(&attrParamP->dataP[sizeof(MODULE_DATA_TYPE)]) = type;
	attrParamP->header.type = ATTR;

	// indent
	Parameter *indentParamP = &messageP->parameters[INDENT_PARAM];
	indentParamP->header.size = INDENT_DATA_SIZE;
	indentParamP->dataP = (uint8_t*)malloc(INDENT_DATA_SIZE);
	assert(indentParamP->dataP != NULL);
	*(INDENT_DATA_TYPE *)(indentParamP->dataP) = indent;
	indentParamP->header.type = INDENT;

	// pid
	Parameter *pidParamP = &messageP->parameters[PID_PARAM];
	pidParamP->header.size = PID_DATA_SIZE;
	pidParamP->dataP = (uint8_t*)malloc(PID_DATA_SIZE);
	assert(pidParamP->dataP != NULL);
	*(PID_DATA_TYPE *)(pidParamP->dataP) = (PID_DATA_TYPE)htonl(pid);
	pidParamP->header.type = PID;

	// tid
	Parameter *tidParamP = &messageP->parameters[TID_PARAM];
	tidParamP->header.size = TID_DATA_SIZE;
	tidParamP->dataP = (uint8_t*)malloc(TID_DATA_SIZE);
	assert(tidParamP->dataP != NULL);
	*(TID_DATA_TYPE *)(tidParamP->dataP) = (TID_DATA_TYPE)htonl(tid);
	tidParamP->header.type = TID;

	// data
	Parameter *dataParamP = &messageP->parameters[DATA_PARAM];
	dataParamP->header.size = size;
	dataParamP->dataP = (uint8_t*)malloc(dataParamP->header.size);
	assert(dataParamP->dataP != NULL);
	memcpy(dataParamP->dataP, dataP, dataParamP->header.size);
	dataParamP->header.type = DATA;

	// timestamp
	Parameter *timeParamP = &messageP->parameters[TIME_PARAM];
	timeParamP->header.size = TIME_DATA_SIZE;
	timeParamP->dataP = (uint8_t*)malloc(TIME_DATA_SIZE);
	assert(timeParamP->dataP != NULL);
	*(TIME_DATA_TYPE *)(timeParamP->dataP) =
 (TIME_DATA_TYPE)htonll((TIME_DATA_TYPE)GetMilliTime());
	timeParamP->header.type = TIME;
	
	// eom
	Parameter *eomParamP = &messageP->parameters[EOM_PARAM];
	eomParamP->header.size = 0;
	eomParamP->dataP = NULL;
	eomParamP->header.type = EOM;

	// create message
	messageP->header.version = (uint16_t)((uint16_t)PROTOCOL_VERSION_MAJOR << 8 |
										 (uint16_t)PROTOCOL_VERSION_MINOR);
	messageP->header.size = MESSAGE_HEADER_SIZE +
						  appNameParamP->header.size + PARAM_HEADER_SIZE +
						  attrParamP->header.size + PARAM_HEADER_SIZE +
						  indentParamP->header.size + PARAM_HEADER_SIZE +
						  pidParamP->header.size + PARAM_HEADER_SIZE +
						  tidParamP->header.size + PARAM_HEADER_SIZE +
						  dataParamP->header.size + PARAM_HEADER_SIZE +
						  timeParamP->header.size + PARAM_HEADER_SIZE +
						  PARAM_HEADER_SIZE;
}

/**
 * \fn static void SendAndFreeMessage(Message *messageP, uint8_t logToFile)
 * \brief sends a protocol formatted message to the reporter application and stores it in log file
 *
 * \param messageP points to a properly formatted message. Message parameters'
 *        data are freed by this function
 * \param logToFile is set if the message must also be saved to the log file
 */
static void SendAndFreeMessage(Message *messageP, uint8_t logToFile) {
	uint32_t u32;
	uint16_t u16;

	if (!messageP)
		return;

	// send complete message because the peer might not parse properly incomplete messages..
	uint8_t* bufferP = (uint8_t*)malloc(messageP->header.size);
	assert(bufferP != NULL);
	int		i = 0;

	// protocol version
	u16 = htons(messageP->header.version);
	memcpy(&bufferP[i], &u16, sizeof(u16));
	i += sizeof(u16);

	// message size
	u32 = htonl(messageP->header.size);
	memcpy(&bufferP[i], &u32, sizeof(u32));
	i += sizeof(u32);

	// parameters
	int p = 0;
	do {
		bufferP[i++] = messageP->parameters[p].header.type;
		u16 = htons(messageP->parameters[p].header.size);
		memcpy(&bufferP[i], &u16, sizeof(u16));
		i += sizeof(u16);
		if (messageP->parameters[p].header.type != EOM) {
			memcpy(&bufferP[i], messageP->parameters[p].dataP, messageP->parameters[p].header.size);
			i += messageP->parameters[p].header.size;

			// free allocated data buffer
			free(messageP->parameters[p].dataP);
		}
	} while (messageP->parameters[p++].header.type != EOM);

	// to tcp
	if (gLogSocket != -1) {
#ifdef WIN32
		if (send((SOCKET)gLogSocket, (const char*)bufferP, messageP->header.size, 0) < 0) {
#else
		if (write(gLogSocket, bufferP, messageP->header.size) < 0) {
#endif
			gConnectionFailed = true;
			gLogSocket = -1;
		}
	}

	// to file
	if (logToFile && gLogFile != -1) {
		uint64_t pos = 0;

		// check log size
		if (gMaxLogSize != -1)
			// stop logging to file if it went too big
#ifdef WIN32
			pos = _lseek(gLogFile, 0, SEEK_END);

#else
			pos = (uint64_t)lseek(gLogFile, 0, SEEK_END);

#endif
		if (gMaxLogSize == -1 || (pos >> 10) < gMaxLogSize) {
#ifdef WIN32
			_write(gLogFile, bufferP, messageP->header.size);
#else
			write(gLogFile, bufferP, messageP->header.size);
#endif
		}
	}

	// free allocated data buffer
	free(bufferP);
}

/**
 * \fn 	int static void DumpLogFile()
 * \brief dump existing log file to the reporter
 *
 * \param appName is the name of the calling application
 */
static void DumpLogFile(const char *appName) {
	if (gLogFile != -1 && gLogSocket != -1) {

		// connected, if we were logging to file, re-send file content over new connection

		// set file r/w offset to 0
#ifdef WIN32
		_lseek(gLogFile, 0, SEEK_SET);

#else
		lseek(gLogFile, 0, SEEK_SET);

#endif
		// send a 'start dump message, so reporter can filter out redundancies'
		Message message;
		BuildMessage(&message, appName, 0, 0, (uint8_t *)START_DUMP_MSG, (uint16_t)strlen(START_DUMP_MSG), START_DUMP_TAG);
		SendAndFreeMessage(&message, 0);

		// dump to reporter
		int bytes;
		unsigned char buffer[1024];
		do {
#ifdef WIN32
			if ((bytes = _read(gLogFile, buffer, sizeof(buffer))) >= 1)
				send(gLogSocket, (const char*)buffer, bytes, 0);
#else
			if ((bytes = read(gLogFile, buffer, sizeof(buffer))) >= 1)
				write(gLogSocket, buffer, bytes);
#endif
		} while (bytes > 0);

		// send a 'end dump message, so reporter can filter out redundancies'
		BuildMessage(&message, appName, 0, 0, (uint8_t *)END_DUMP_MSG, (uint16_t)strlen(END_DUMP_MSG), END_DUMP_TAG);
		SendAndFreeMessage(&message, 0);

		// truncate file
#ifdef WIN32
		_chsize(gLogFile, 0);
#else
		ftruncate(gLogFile, 0);

#endif
	}
}

/**
 * \fn static int InitLogFile()
 * \ brief creates the log file if it doesn't exist yet
 *
 * \param appName is the name of the calling application
 *
 * \return 0 on success, -1 else
 */
static int InitLogFile(const char *appName) {
	// if logging, keep going
	if (gLogFile != -1 || gMaxLogSize == 0)
		return 0;

	// tries to open/create log file

#ifdef WIN32
	char logName[MAX_PATH + 1];
	snprintf(logName, sizeof(logName), "%s%s", appName, LOG_FILE_EXTENSION);
	_sopen_s(&gLogFile, logName, O_APPEND | O_CREAT | O_RDWR, _SH_DENYNO, _S_IWRITE);

#else
	char logName[PATH_MAX + 1];
	snprintf(logName, sizeof(logName), "%s%s", appName, LOG_FILE_EXTENSION);
	gLogFile = open(logName, O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

#endif
	return gLogFile > 0 ? 0 : -1;
}

/**
 * \fn static int InitConnection()
 * \brief initializes the connection to the reporter application if not done yet. If a log file present
 *        when setting up a new connection, dumps the log file to the peer log-reporter
 *
 * \param appName is the name of the calling application
 *
 * \return 0 on success, -1 else
 */
static int InitConnection(const char *appName) {
	// if logging, keep going
	if (gLogSocket != -1 || (!gAutoReconnect && gConnectionFailed))
		return 0;

#ifdef WIN32
	if (!gWSAStartupDone) { 
		gWSAStartupDone = true;
		
		// more windows crap..
		#pragma comment( lib, "ws2_32.lib")
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
			return -1;
	}
#endif
	
	struct 	sockaddr_in serverAddr;
	socklen_t addr_size;

	/*---- Create the socket. The three arguments are: ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
#ifdef WIN32
	SOCKET fd = socket(PF_INET, SOCK_STREAM, 0);

#else
	int fd = socket(PF_INET, SOCK_STREAM, 0);

#endif
	/*---- Configure settings of the server address struct ----*/
	/* Address family = Internet */
	serverAddr.sin_family = AF_INET;

	/* Set port number, using htons function to use proper byte order */
#ifdef WIN32
	serverAddr.sin_port = (uint16_t)htons(gServerPort);

#else
	serverAddr.sin_port = htons(gServerPort);

#endif
	/* Set IP address to localhost */
	serverAddr.sin_addr.s_addr = inet_addr(gServerIpAddress);

	/* Set all bits of the padding field to 0 */
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	/*---- Connect the socket to the server using the address struct ----*/
	addr_size = sizeof serverAddr;
	if (!connect(fd, (struct sockaddr *) &serverAddr, addr_size)) {
		gConnectionFailed = false;
		gLogSocket = (int)fd;
		DumpLogFile(appName);
	} else 
		gConnectionFailed = true;

	return gLogSocket > 0 ? 0 : -1;
}

/**
 * \fn static int InitLog()
 * \ brief creates the connection and/or the the log file if not done yet
 *
 * \param appName is the name of the calling application
 *
 * \return 0 on success (at least one of the log routes was opened), -1 else
 */
static int InitLog(const char *appName) {
	int result = InitLogFile(appName);
	result &= InitConnection(appName);
	return result;
}

/**
 * \fn void SetLogAddress(const char *ipAddress, uint16_t port)
 * \brief set the IP (either IPV4 XXX.XXX.XXX.XXX numeric form or host name) address of the log-reporter
 * 		  server. 	This function overrides the settings file's address
 *
 * \param ipAddress is the IP address of the log-reporter server
 * \param port is the associated port number
 */
void SetLogAddress(const char *ipAddress, uint16_t port) {
	if (!ipAddress)
		return;

	// reset connection
	if (gLogSocket != -1) {

#ifdef WIN32
		closesocket(gLogSocket);
#else
		close(gLogSocket);
#endif
		gLogSocket = (int)-1;
	}

    	struct hostent *he;
    	struct in_addr **addr_list;

   	if (!(he = gethostbyname(ipAddress))) {  // get the host info
		fprintf(stderr, "couldn't get host name for %s!\n", ipAddress);
		return;
    	}

	// store new address
    	addr_list = (struct in_addr **)he->h_addr_list;


#ifdef WIN32
	strncpy_s(gServerIpAddress, sizeof(gServerIpAddress), inet_ntoa(*addr_list[0]), strlen(inet_ntoa(*addr_list[0])));
#else
	strncpy(gServerIpAddress, inet_ntoa(*addr_list[0]), sizeof(gServerIpAddress));
#endif
	gServerIpAddress[sizeof(gServerIpAddress) -1] = '\0';
	gServerPort = port;

    	// print information about this host
	printf("Set LogReporter address to %s:%d (%s:%d)\n", he->h_name, gServerPort, gServerIpAddress, gServerPort);
}

/**
 * \fn static void BuildAndSendMessage(const char *appName, uint32_t module, uint8_t indent, const uint8_t *dataP, uint16_t size, TYPE_DATA_TYPE type)
 * \brief build a protocol log message and send it to the connected log-reporter server
 *
 * \param appName is the either the fully qualified or base name of the executable issuing the call. Can be used to filter in/colorize
 *        the log message
 * \param module is the module bit mask to be used to filter in/colorise the log message
 * \param indent is the indentation level for the log message
 * \param dataP points to the log message data, can be either a C string or a binary array depending on the data type
 * \param type specifies the type of data pointed by dataP
 * \param size is the length of the binary array referenced by dataP
 */
static void BuildAndSendMessage(const char *appName, uint32_t module, uint8_t indent, const uint8_t *dataP, uint16_t size, TYPE_DATA_TYPE type) {
	// create message
	Message message;
	BuildMessage(&message, appName, module, indent, dataP, size, type);

	// send and free message
	SendAndFreeMessage(&message, (uint8_t)(gMaxLogSize > 0));
}

/**
 * \fn void LogText(const char *appName, uint32_t module, uint8_t indent, const char *textP, uint8_t lineBreak)
 * \brief logs a message
 *
 * \param module is the module bit mask to be used to filter in/colorise the log message
 * \param indent is the indentation level for the log message
 * \param lineBreak is true if a line break shall happen before the next message
 * \param textP points to the C string to be sent
 */
void LogText(uint32_t module, uint8_t indent, bool lineBreak, const char *textP) {
	if (!textP)
		return;

	// get calling process name
	const char *appName = GetProcessName();

	// load settings from setting file
	InitSettings(appName);

	if (gTracesOff)
		return;

	// skip message which module don't match the log module
	if (!(module & gModIdLogMask))
		return;

	// init comm and log file
	if (InitLog(appName))
		return;

	BuildAndSendMessage(appName, module, indent, (const uint8_t *)textP, (uint16_t)strlen(textP), (TYPE_DATA_TYPE)(lineBreak ? MULTILINE : SINGLELINE));
}

/**
 * \fn void LogData(const char *appName, uint32_t module, uint8_t indent, const char *dataP, uint16_t size)
 * \brief logs a message
 *
 * \param module is the module bit mask to be used to filter in/colorise the log message
 * \param indent is the indentation level for the log message
 * \param dataP points to the binary array to be sent
 * \param size is the length of the binary array referenced by dataP
 */
void LogData(uint32_t module, uint8_t indent, const uint8_t *dataP, uint16_t size) {
	if (!dataP)
		return;

	// get calling process name
	const char *appName = GetProcessName();

	// load settings from setting file
	InitSettings(appName);

	if (gTracesOff)
		return;

	// skip message which module don't match the log module
	if (!(module & gModIdLogMask))
		return;

	// init comm and log file
	if (InitLog(appName))
		return;

	BuildAndSendMessage(appName, module, indent, dataP, size, BINARY);
}

/**
 * \fn void LogVText(const char *appName, uint32_t module, uint8_t indent, uint8_t lineBreak, const char *formatP, )
 * \brief logs a message
 *
 * \param module is the module bit mask to be used to filter in/colorise the log message
 * \param indent is the indentation level for the log message
 * \param lineBreak is true if a line break shall happen before the next message
 * \param formatP points to the format string as expected by the s*printf functions familly
 */
static char message[1024 + 1];
void LogVText(uint32_t module, uint8_t indent, bool lineBreak, const char *formatP, ...) {
	va_list vl;
	va_start(vl, formatP);
	vsnprintf(message, sizeof(message), formatP, vl);
	va_end(vl);

	LogText(module, indent, lineBreak, message);
}


#ifdef WIN32
#undef _WINSOCK_DEPRECATED_NO_WARNINGS 
#endif
