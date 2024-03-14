#include <thread>

#include <stdio.h>
#include <stdarg.h>
#include <Link.h>
#include <iostream>
#include <log2reporter.h>

#include "RemoteProcedureCall.h"

using namespace std;

const unordered_map<string, RemoteProcedureCall::ParamType> RemoteProcedureCall::m_types = {
	{"ptr",			PTR},
	{"BYTE",		BYTE},
	{"BYTE_REF",	BYTE_REF},
	{"CHAR",		CHAR},
	{"CHAR_REF",	CHAR_REF},
	{"INT16",		INT16},
	{"INT16_REF",	INT16_REF},
	{"UINT16",		UINT16},
	{"UINT16_REF",	UINT16_REF},
	{"INT32",		INT32},
	{"INT32_REF",	INT32_REF},
	{"UINT32",		UINT32},
	{"UINT32_REF",	UINT32_REF},
	{"INT64",		INT64},
	{"INT64_REF",	INT64_REF},
	{"UINT64",		UINT64},
	{"UINT64_RFE",	UINT64_REF},
	{"DOUBLE",		DOUBLE},
	{"DOUBLE_REF",	DOUBLE_REF},
	{"STRING",		STRING},
	{"STRING_REF",	STRING_REF},
	{"RESULT_ADDRESS", RESULT_ADDRESS},
	{"END_OF_CALL", END_OF_CALL},
	{"ASYNC_ID",	ASYNC_ID},
	{"EMPTY",		EMPTY}
};

/**
 * \fn bool RemoteProcedureCall::SendPacket(unsigned char* bufferP, unsigned long dataLen)
 * \brief Sends a data packet over the transport link.
 * 
 * \param bufferP contains the data to be sent
 * \param dataLen is the number of bytes to sent from bufferP
 * 
 * \return (bool)ReturnValue is true if data could be sent, false else (and the
 *		   error code is set).
*/
ReturnValue<bool, CommunicationErrors> RemoteProcedureCall::SendPacket(unsigned char* bufferP, unsigned long dataLen) {
	ReturnValue<bool, CommunicationErrors> r;
	uint64_t 							   len = dataLen;
	unsigned char 						   buffer[sizeof(uint64_t)];

	if (len == 0 || !bufferP) {
		r = {false, CommunicationErrors::ErrorCode::BadArgument};
		return r;
	}

	// encode the packet length
	int offset = 0;
	encode_uint64(HTONLL(len), buffer, offset);

	ReturnValue<bool, CommunicationErrors> rv;

	// send the packet len
	if ((rv = m_linkP->Send(buffer, (unsigned long)sizeof(len))).IsError()) 
		return rv;

	// send the whole call stream
	if ((rv = m_linkP->Send(bufferP, dataLen)).IsError())
		return rv;

	r = { true, CommunicationErrors::ErrorCode::None };
	return r;
}

/**
 * \fn unsigned char* RemoteProcedureCall::ReceivePacket(unsigned long& dataLen)
 * \brief Receives a data packet over the transport link.
 * 
 * \param dataLen is the number of bytes to receive. This is blocking call!
 * 
 * \return if no error was encountered, (unsigned char*)ReturnValue is a newly 
 *		   (m)allocated buffer pointing the received data. This buffer must be freed by the caller. 
 *		   Returns (unsigned char*)ReturnValue == NULL if an error occured.
*/
ReturnValue<unsigned char*, CommunicationErrors> RemoteProcedureCall::ReceivePacket(unsigned long& dataLen) {
	ReturnValue<unsigned char*, CommunicationErrors> r;
	unsigned char*								     bufferP;
	uint64_t 									     len;
	unsigned char 								     buffer[sizeof(uint64_t)];

	// retrieve the packet len
	ReturnValue<bool, CommunicationErrors> rv;
	if ((rv = m_linkP->Receive(buffer, (unsigned long)sizeof(len))).IsError()) {
		r = { NULL, rv.GetError<CommunicationErrors::ErrorCode>() };
		return r;
	}

	int offset = 0;
	len = decode_uint64(buffer, offset);
	if ((dataLen = (unsigned long)NTOHLL(len)) == 0) {
		r = { NULL, CommunicationErrors::ErrorCode::MissingData };
		return r;
	}

	// allocate the necessary bufferP
	bufferP = (unsigned char*)malloc(dataLen);
	if (!bufferP) {
		r = { bufferP, CommunicationErrors::ErrorCode::AllocationError };
		return r;
	}

	// wait and read the whole call stream
	if ((rv = m_linkP->Receive(bufferP, (unsigned long)dataLen)).IsError()) {
		free(bufferP);
		r = { NULL, rv.GetError<CommunicationErrors::ErrorCode>() };
		return r;
	}

	r = { bufferP, CommunicationErrors::ErrorCode::None };
	return r;
}

/**
 * \fn void RemoteProcedureCall::push_int64(vector<unsigned char>& v, int64_t val)
 * \brief pushes an int64 into the given serialization vector.
 *
 * \param v is a byte vector containing the serialized call stream
 * \param val is the data to serialize into the vector v
 */
inline void RemoteProcedureCall::push_int64(vector<unsigned char>& v, int64_t val) {
	v.push_back((unsigned char)((val & 0xFF00000000000000ull) >> 56));
	v.push_back((unsigned char)((val & 0x00FF000000000000ull) >> 48));
	v.push_back((unsigned char)((val & 0x0000FF0000000000ull) >> 40));
	v.push_back((unsigned char)((val & 0x000000FF00000000ull) >> 32));
	v.push_back((unsigned char)((val & 0x00000000FF000000ull) >> 24));
	v.push_back((unsigned char)((val & 0x0000000000FF0000ull) >> 16));
	v.push_back((unsigned char)((val & 0x000000000000FF00ull) >> 8));
	v.push_back((unsigned char)(val & 0x00000000000000FFull));
}

/**
 * \fn void RemoteProcedureCall::push_uint64(vector<unsigned char>& v, uint64_t val)
 * \brief pushes a uint64 into the given serialization vector.
 *
 * \param v is a byte vector containing the serialized call stream
 * \param val is the data to serialize into the vector v
 */
inline void RemoteProcedureCall::push_uint64(vector<unsigned char>& v, uint64_t val) {
	v.push_back((unsigned char)((val & 0xFF00000000000000ull) >> 56));
	v.push_back((unsigned char)((val & 0x00FF000000000000ull) >> 48));
	v.push_back((unsigned char)((val & 0x0000FF0000000000ull) >> 40));
	v.push_back((unsigned char)((val & 0x000000FF00000000ull) >> 32));
	v.push_back((unsigned char)((val & 0x00000000FF000000ull) >> 24));
	v.push_back((unsigned char)((val & 0x0000000000FF0000ull) >> 16));
	v.push_back((unsigned char)((val & 0x000000000000FF00ull) >> 8));
	v.push_back((unsigned char)(val & 0x00000000000000FFull));
}

/**
 * \fn void RemoteProcedureCall::push_int32(vector<unsigned char>& v, int32_t val)
 * \brief pushes an int32 into the given serialization vector.
 *
 * \param v is a byte vector containing the serialized call stream
 * \param val is the data to serialize into the vector v
 */
inline void RemoteProcedureCall::push_int32(vector<unsigned char>& v, int32_t val) {
	v.push_back((unsigned char)((val & 0xFF000000) >> 24));
	v.push_back((unsigned char)((val & 0x00FF0000) >> 16));
	v.push_back((unsigned char)((val & 0x0000FF00) >> 8));
	v.push_back((unsigned char)(val & 0x000000FF));
}

/**
 * \fn void RemoteProcedureCall::push_uint32(vector<unsigned char>& v, uint32_t val)
 * \brief pushes a uint32 into the given serialization vector.
 *
 * \param v is a byte vector containing the serialized call stream
 * \param val is the data to serialize into the vector v
 */
inline void RemoteProcedureCall::push_uint32(vector<unsigned char>& v, uint32_t val) {
	v.push_back((unsigned char)((val & 0xFF000000) >> 24));
	v.push_back((unsigned char)((val & 0x00FF0000) >> 16));
	v.push_back((unsigned char)((val & 0x0000FF00) >> 8));
	v.push_back((unsigned char)(val & 0x000000FF));
}

/**
 * \fn void RemoteProcedureCall::push_int16(vector<unsigned char>& v, int16_t val)
 * \brief pushes an int16 into the given serialization vector.
 *
 * \param v is a byte vector containing the serialized call stream
 * \param val is the data to serialize into the vector v
 */
inline void RemoteProcedureCall::push_int16(vector<unsigned char>& v, int16_t val) {
	v.push_back((unsigned char)((val & 0xFF00) >> 8));
	v.push_back((unsigned char)(val & 0x00FF));
}

/**
 * \fn void RemoteProcedureCall::push_uint16(vector<unsigned char>& v, uint16_t val)
 * \brief pushes a uint16 into the given serialization vector.
 *
 * \param v is a byte vector containing the serialized call stream
 * \param val is the data to serialize into the vector v
 */
inline void RemoteProcedureCall::push_uint16(vector<unsigned char>& v, uint16_t val) {
	v.push_back((unsigned char)((val & 0xFF00) >> 8));
	v.push_back((unsigned char)(val & 0x00FF));
}

/**
 * \fn unsigned long RemoteProcedureCall::PrepareSerializeCall(AsyncID asyncId, const string& function, vector<unsigned char>& serializedCall, shared_ptr<unsigned long> result, va_list vl)
 * \brief Serializes the given function call so the resulting byte stream can 
 *		  be sent over the associated link using SendSerializedCall.
 *
 *        The stream format is:
 *	        	ASYNC_ID.<asyncId>.STRING.<function>.UINT64.<result_address>.<parameters>.END_OF_CALL
 *  	      	result_address : address of the UINT64 to receive return_value on the
 *                         		 caller side
 *        		parameters := ((<type>.<value>)|(PTR.<type>.<ptr_value>.<value>))*
 *        		ptr_value := address of the location to receive value on the caller
 *                     		 side
 *        		type := see ParamType in .h
 *        		value := parameter value
 *        				 string values are streamed as uint16_t len followed by non asciiz
 *        				 string content
 *
 * \param asyncId is asynchronous call identifier, which will be returned with
 *				 the result to match it. It is O if the call is synchronous
 * \param function is the name of the function to call
 * \param serializedCall is the serialized vector of parameters to build with the vl list of 
 * \param resultP points to the variable which will receive the server's procedure return value
 *		  arguments
 * 		  (<type><value>)|(PTR<pointer_to_value>). The last argument must be
 * 		  'END_OF_CALL'.
 *        STRING values MUST be passed as string pointers (std::string*)
 */
void RemoteProcedureCall::PrepareSerializeCall(AsyncID asyncId, const string& function, vector<unsigned char>& serializedCall, unsigned long* resultP, va_list vl) {
	unsigned int 	len;
	string			s;
	unsigned char	byte;
	char			c;
	int16_t			i16;
	uint16_t		ui16;
	int32_t			i32;
	uint32_t		ui32;
	int64_t			i64;
	uint64_t		ui64;
	double			d;
	bool 			is_ptr = false;
	void*			ptr;

#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 0, true, "RemoteProcedureCall::PrepareSerializeCall(%lu, %s, ...)", asyncId, function.c_str());
#endif

	// first pass the asyncId
	serializedCall.push_back(ASYNC_ID);
	push_uint64(serializedCall, HTONLL((uint64_t)asyncId));

	// then pass the function name
	serializedCall.push_back(STRING);
	len = (unsigned int)function.length();
	push_uint16(serializedCall, htons(len));
	unsigned char* p = (unsigned char*)function.c_str();
	serializedCall.insert(serializedCall.end(), p, p+len);
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "pushed function %s", function.c_str());
#endif

	// then add the return result address
	serializedCall.push_back(UINT64);
	push_uint64(serializedCall, HTONLL((uint64_t)resultP));
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "pushed return address %p", resultP);
#endif

	// and all the given parameters
	for (;;) {
		ParamType type = (ParamType)va_arg(vl, int);

		// first push the parameter type in the call stream vector
		serializedCall.push_back(type);
#ifdef RPC_TRACES
		LogVText(RPC_MODULE, 6, true, "pushed parameter type %x", type);
#endif
		// we're done when encountering this parameter
		if (type == END_OF_CALL)
			break;

		// then, the corresponding data
		switch (type) {
			case PTR:
				// next parameter is a pointer
				is_ptr = true;
				break;

			case BYTE:
				if (is_ptr) {
					is_ptr = false;

					// a byte ptr
					ptr = (void*)va_arg(vl, unsigned char*);
					push_uint64(serializedCall, HTONLL((uint64_t)ptr));
					serializedCall.push_back(ptr ? *(unsigned char*)ptr : 0);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter byte ptr %p", ptr);
#endif
				} else {
					// a byte
					byte = (unsigned char)va_arg(vl, int);
					serializedCall.push_back(byte);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter byte %u", byte);
#endif
				}
				break;

			case CHAR:
				if (is_ptr) {
					is_ptr = false;

					// a char ptr
					ptr = (void*)va_arg(vl, char*);
					push_uint64(serializedCall, HTONLL((uint64_t)ptr));
					serializedCall.push_back(ptr ? *(char*)ptr : 0);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter char ptr %p", ptr);
#endif
				} else {
					// a char
					c = (char)va_arg(vl, int);
					serializedCall.push_back(c);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter char %c", c);
#endif
				}
				break;

			case INT16:
				if (is_ptr) {
					is_ptr = false;

					// a short integer ptr
					ptr = (void*)va_arg(vl, int16_t*);
					push_uint64(serializedCall, HTONLL((uint64_t)ptr));
					i16 = ptr ? *(int16_t*)ptr : 0;
					push_int16(serializedCall, ntohs(i16));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter int16 ptr %p", ptr);
#endif
				} else {
					// a short integer
					i16 = (int16_t)va_arg(vl, int);
					push_int16(serializedCall, htons(i16));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter int16_t %ud", i16);
#endif
				}
				break;

			case UINT16:
				if (is_ptr) {
					is_ptr = false;

					// an unsigned short ptr
					ptr = (void*)va_arg(vl, uint16_t*);
					push_uint64(serializedCall, HTONLL((uint64_t)ptr));
					ui16 = ptr ? *(uint16_t*)ptr : 0;
					push_uint16(serializedCall, ntohs(ui16));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter uint16_t ptr %p", ptr);
#endif
				} else {
					// an unsigned short integer
					ui16 = (uint16_t)va_arg(vl, int);
					push_uint16(serializedCall, htons(ui16));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter uint16_t %u", ui16);
#endif
				}
				break;

			case INT32:
				if (is_ptr) {
					is_ptr = false;

					// a long integer ptr
					ptr = (void*)va_arg(vl, int32_t*);
					push_uint64(serializedCall, HTONLL((uint64_t)ptr));
					i32 = ptr ? *(int32_t*)ptr : 0;
					push_int32(serializedCall, ntohl(i32));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter int32_t ptr %p", ptr);
#endif
				} else {
					// a long integer
					i32 = (int32_t)va_arg(vl, int32_t);
					push_int32(serializedCall, htonl(i32));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter int32_t %d", i32);
#endif
				}
				break;

			case UINT32:
				if (is_ptr) {
					is_ptr = false;

					// an unsigned long ptr
					ptr = (void*)va_arg(vl, uint32_t*);
					push_uint64(serializedCall, HTONLL((uint64_t)ptr));
					ui32 = ptr ? *(uint32_t*)ptr : 0;
					push_uint32(serializedCall, ntohl(ui32));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter uint32_t ptr %p", ptr);
#endif
				} else {
					// an unsigned long integer
					ui32 = (uint32_t)va_arg(vl, uint32_t);
					push_uint32(serializedCall, htonl(ui32));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter uint32_t %u", ui32);
#endif
				}
				break;

			case INT64:
				if (is_ptr) {
					is_ptr = false;

					// a long long ptr
					ptr = (void*)va_arg(vl, int64_t*);
					push_uint64(serializedCall, HTONLL((uint64_t)ptr));
					i64 = ptr ? *(int64_t*)ptr : 0;
					push_int64(serializedCall, NTOHLL(i64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter int64_t ptr %p", ptr);
#endif
				} else {
					// a long long integer
					i64 = va_arg(vl, int64_t);
					push_int64(serializedCall, HTONLL(i64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter int64_t %ld", i64);
#endif
				}
				break;

			case UINT64:
				if (is_ptr) {
					is_ptr = false;

					// an unsigned long long ptr
					ptr = (void*)va_arg(vl, uint64_t*);
					push_uint64(serializedCall, HTONLL((uint64_t)ptr));
					ui64 = ptr ? *(uint64_t*)ptr : 0;
					push_uint64(serializedCall, NTOHLL(ui64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter uint64_t ptr %p", ptr);
#endif
				} else {
					// an unsigned long long integer
					ui64 = va_arg(vl, uint64_t);
					push_uint64(serializedCall, HTONLL(ui64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter uint64_t %lu", ui64);
#endif
				}
				break;

			case DOUBLE:
				if (is_ptr) {
					is_ptr = false;

					// a double ptr
					ptr = (void*)va_arg(vl, double*);
					push_uint64(serializedCall, HTONLL((uint64_t)ptr));
					ui64 = ptr ? *(uint64_t*)ptr : 0;
					push_uint64(serializedCall, NTOHLL(ui64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter double ptr %p", ptr);
#endif
				} else {
					// a double
					d = (double)va_arg(vl, double);
					ui64 = *(uint64_t*)&d;
					push_uint64(serializedCall, HTONLL(ui64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter double %f", d);
#endif
				}
				break;

			case STRING:
				if (is_ptr) {
					is_ptr = false;

					// a string ptr
					ptr = (void*)va_arg(vl, string*);
					push_uint64(serializedCall, HTONLL((uint64_t)ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter string %p", ptr);
#endif
					// string
					s = ptr ? *(string*)ptr : "";
					len = (unsigned int)s.length();
					push_uint16(serializedCall, htons(len));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter string len %u", len);
#endif
					unsigned char* p = (unsigned char*)s.c_str();
					serializedCall.insert(serializedCall.end(), p, p+len);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter string %s", s.c_str());
#endif
				} else {
					// string
					s = *va_arg(vl, string*);
					len = (unsigned int)s.length();
					push_uint16(serializedCall, htons(len));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter string len %u", len);
#endif
					unsigned char* p = (unsigned char*)s.c_str();
					serializedCall.insert(serializedCall.end(), p, p+len);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter string %s", s.c_str());
#endif
				}
				break;
		}
	}
}

void RemoteProcedureCall::PrepareSerializeCall(AsyncID asyncId, const string& function, vector<unsigned char>& serializedCall, unsigned long* resultP, vector<RemoteProcedureCall::ParameterBase*>* paramsP) {
	unsigned int 	len;
	string			s;
	unsigned char	byte;
	char			c;
	int16_t			i16;
	uint16_t		ui16;
	int32_t			i32;
	uint32_t		ui32;
	int64_t			i64;
	uint64_t		ui64;
	double			d;
	bool 			is_ptr = false;
	void*			ptr;

#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 0, true, "RemoteProcedureCall::PrepareSerializeCall(%lu, %s, ...)", asyncId, function.c_str());
#endif

	// first pass the asyncId
	serializedCall.push_back(ASYNC_ID);
	push_uint64(serializedCall, HTONLL((uint64_t)asyncId));

	// then pass the function name
	serializedCall.push_back(STRING);
	len = (unsigned int)function.length();
	push_uint16(serializedCall, htons(len));
	unsigned char* p = (unsigned char*)function.c_str();
	serializedCall.insert(serializedCall.end(), p, p + len);
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "pushed function %s", function.c_str());
#endif

	// then add the return result address
	serializedCall.push_back(UINT64);
	push_uint64(serializedCall, HTONLL((uint64_t)resultP));
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "pushed return address %p", resultP);
#endif

	// and all the given parameters
	for (ParameterBase* paramP : *paramsP) {
		ParamType type = paramP->GetType();

		// first push the parameter type in the call stream vector
		serializedCall.push_back(type);
#ifdef RPC_TRACES
		LogVText(RPC_MODULE, 6, true, "pushed parameter type %x", type);
#endif
		// we're done when encountering this parameter
		if (type == END_OF_CALL)
			break;

		// then, the corresponding data
		switch (type) {
		case PTR:
			is_ptr = true;
			break;

		case BYTE:
			if (is_ptr) {
				is_ptr = false;

				// a byte ptr
				ptr = (void*)paramP->GetCallerPointer();
				push_uint64(serializedCall, HTONLL((uint64_t)ptr));
				serializedCall.push_back(ptr ? *(unsigned char*)ptr : 0);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter byte ptr %p", ptr);
#endif
			}
			else {
				// a byte
				byte = ParameterSafeCast(uint8_t, paramP)->GetReference();
				serializedCall.push_back(byte);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter byte %u", byte);
#endif
			}
			break;

		case CHAR:
			if (is_ptr) {
				is_ptr = false;

				// a char ptr
				ptr = (void*)paramP->GetCallerPointer();
				push_uint64(serializedCall, HTONLL((uint64_t)ptr));
				serializedCall.push_back(ptr ? *(char*)ptr : 0);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter char ptr %p", ptr);
#endif
			}
			else {
				// a char
				c = ParameterSafeCast(int, paramP)->GetReference();
				serializedCall.push_back(c);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter char %c", c);
#endif
			}
			break;

		case INT16:
			if (is_ptr) {
				is_ptr = false;

				// a short integer ptr
				ptr = (void*)paramP->GetCallerPointer();
				push_uint64(serializedCall, HTONLL((uint64_t)ptr));
				i16 = ptr ? *(int16_t*)ptr : 0;
				push_int16(serializedCall, ntohs(i16));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter int16 ptr %p", ptr);
#endif
			}
			else {
				// a short integer
				i16 = ParameterSafeCast(int16_t, paramP)->GetReference();
				push_int16(serializedCall, htons(i16));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter int16_t %ud", i16);
#endif
			}
			break;

		case UINT16:
			if (is_ptr) {
				is_ptr = false;

				// an unsigned short ptr
				ptr = (void*)paramP->GetCallerPointer();
				push_uint64(serializedCall, HTONLL((uint64_t)ptr));
				ui16 = ptr ? *(uint16_t*)ptr : 0;
				push_uint16(serializedCall, ntohs(ui16));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter uint16_t ptr %p", ptr);
#endif
			}
			else {
				// an unsigned short integer
				ui16 = ParameterSafeCast(uint16_t, paramP)->GetReference();
				push_uint16(serializedCall, htons(ui16));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter uint16_t %u", ui16);
#endif
			}
			break;

		case INT32:
			if (is_ptr) {
				is_ptr = false;

				// a long integer ptr
				ptr = (void*)paramP->GetCallerPointer();
				push_uint64(serializedCall, HTONLL((uint64_t)ptr));
				i32 = ptr ? *(int32_t*)ptr : 0;
				push_int32(serializedCall, ntohl(i32));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter int32_t ptr %p", ptr);
#endif
			}
			else {
				// a long integer
				i32 = ParameterSafeCast(int32_t, paramP)->GetReference();
				push_int32(serializedCall, htonl(i32));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter int32_t %d", i32);
#endif
			}
			break;

		case UINT32:
			if (is_ptr) {
				is_ptr = false;

				// an unsigned long ptr
				ptr = (void*)paramP->GetCallerPointer();
				push_uint64(serializedCall, HTONLL((uint64_t)ptr));
				ui32 = ptr ? *(uint32_t*)ptr : 0;
				push_uint32(serializedCall, ntohl(ui32));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter uint32_t ptr %p", ptr);
#endif
			}
			else {
				// an unsigned long integer
				ui32 = ParameterSafeCast(uint32_t, paramP)->GetReference();
				push_uint32(serializedCall, htonl(ui32));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter uint32_t %u", ui32);
#endif
			}
			break;

		case INT64:
			if (is_ptr) {
				is_ptr = false;

				// a long long ptr
				ptr = (void*)paramP->GetCallerPointer();
				push_uint64(serializedCall, HTONLL((uint64_t)ptr));
				i64 = ptr ? *(int64_t*)ptr : 0;
				push_int64(serializedCall, NTOHLL(i64));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter int64_t ptr %p", ptr);
#endif
			}
			else {
				// a long long integer
				i64 = ParameterSafeCast(int64_t, paramP)->GetReference();
				push_int64(serializedCall, HTONLL(i64));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter int64_t %ld", i64);
#endif
			}
			break;

		case UINT64:
			if (is_ptr) {
				is_ptr = false;

				// an unsigned long long ptr
				ptr = (void*)paramP->GetCallerPointer();
				push_uint64(serializedCall, HTONLL((uint64_t)ptr));
				ui64 = ptr ? *(uint64_t*)ptr : 0;
				push_uint64(serializedCall, NTOHLL(ui64));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter uint64_t ptr %p", ptr);
#endif
			}
			else {
				// an unsigned long long integer
				ui64 = ParameterSafeCast(uint64_t, paramP)->GetReference();
				push_uint64(serializedCall, HTONLL(ui64));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter uint64_t %lu", ui64);
#endif
			}
			break;

		case DOUBLE:
			if (is_ptr) {
				is_ptr = false;

				// a double ptr
				ptr = (void*)paramP->GetCallerPointer();
				push_uint64(serializedCall, HTONLL((uint64_t)ptr));
				ui64 = ptr ? *(uint64_t*)ptr : 0;
				push_uint64(serializedCall, NTOHLL(ui64));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter double ptr %p", ptr);
#endif
			}
			else {
				// a double
				d = ParameterSafeCast(double, paramP)->GetReference();
				ui64 = *(uint64_t*)&d;
				push_uint64(serializedCall, HTONLL(ui64));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter double %f", d);
#endif
			}
			break;

		case STRING:
			if (is_ptr) {
				is_ptr = false;

				// a string ptr
				ptr = (void*)paramP->GetCallerPointer();
				push_uint64(serializedCall, HTONLL((uint64_t)ptr));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter string %p", ptr);
#endif
				// string
				s = ptr ? *(string*)ptr : "";
				len = (unsigned int)s.length();
				push_uint16(serializedCall, htons(len));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter string len %u", len);
#endif
				unsigned char* p = (unsigned char*)s.c_str();
				serializedCall.insert(serializedCall.end(), p, p + len);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter string %s", s.c_str());
#endif
			}
			else {
				// string
				s = ParameterSafeCast(string, paramP)->GetReference();
				len = (unsigned int)s.length();
				push_uint16(serializedCall, htons(len));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter string len %u", len);
#endif
				unsigned char* p = (unsigned char*)s.c_str();
				serializedCall.insert(serializedCall.end(), p, p + len);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed parameter string %s", s.c_str());
#endif
			}
			break;
		}
	}
}

/**
 * \fn unsigned long RemoteProcedureCall::SendSerializeCall(AsyncID asyncId, AsyncReplyProcedure* procedureP, vector<unsigned char>& serializedCall, shared_ptr<unsigned long> result)
 * \brief Send the serialized call stream prepared by PrepareSerializeCall to the server.
 *
 * \param asyncId is asynchronous call identifier, which will be returned with
 *				 the result to match it. It is O if the call is synchronous
 * \param serializedCall is the serialized vector of parameters to build with the vl list of
 *		  arguments
 * 
 * \return (bool)ReturnValue is true if data could be sent, false else (and the
 *		   error code is set).
 */
ReturnValue<bool, CommunicationErrors> RemoteProcedureCall::SendSerializedCall(AsyncID asyncId, vector<unsigned char>& serializedCall) {
	ReturnValue<bool, CommunicationErrors> r;

#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 0, true, "RemoteProcedureCall::SendSerializedCall(%lu, ...)", asyncId);
#endif

	// send all the serialized call parameters over to the peer
	unsigned long buff_len = (unsigned long)serializedCall.size();
	{
		unique_lock<mutex> lock(m_cli_send_mutex);
		if ((r = SendPacket((unsigned char*)serializedCall.data(), buff_len)).IsError()) 
			return r;
	}

#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "sent %lu bytes. Will now wait for reply...", buff_len);
#endif

	// synchronous call, blocking until we have a reply from the server
	AsyncID		   dummy;
	unsigned char* bufferP;
	{
		unique_lock<mutex> lock(m_cli_receive_mutex);
		ReturnValue<unsigned char*, CommunicationErrors> rv;
		if ((rv = ReceivePacket(buff_len)).IsError()) {
			r = ReturnValue < bool, CommunicationErrors>{false, rv.GetError<CommunicationErrors::ErrorCode>()};
			return r; 
		} 

		bufferP = rv.GetResult();
	}

	r = DeserializeCallReturn(dummy, bufferP); 
	free(bufferP);

	return r;
}

/**
 * \fn int64_t RemoteProcedureCall::decode_int64(unsigned char* bufferP, int& offset)
 * \brief return the int64 value contained at &bufferP[offset] and increment
 *        the offset by the appropriate number of bytes.
 *
 * \param bufferP contains data serialized by SerializeCall or SerializeCallReturn.
 * \param offset is the index to the start of the value to decode in bufferP
 */
inline int64_t RemoteProcedureCall::decode_int64(unsigned char* bufferP, int& offset) {
	int64_t i64 = int64_t(bufferP[offset++]) << 56;
	i64 |= int64_t(bufferP[offset++]) << 48;
	i64 |= int64_t(bufferP[offset++]) << 40;
	i64 |= int64_t(bufferP[offset++]) << 32;
	i64 |= int64_t(bufferP[offset++]) << 24;
	i64 |= int64_t(bufferP[offset++]) << 16;
	i64 |= int64_t(bufferP[offset++]) << 8;
	i64 |= bufferP[offset++];

	return i64;
}

inline void RemoteProcedureCall::encode_uint64(uint64_t val, unsigned char* bufferP, int& offset) {
	bufferP[offset++] = (unsigned char)((val & 0xFF00000000000000ull) >> 56);
	bufferP[offset++] = (unsigned char)((val & 0x00FF000000000000ull) >> 48);
	bufferP[offset++] = (unsigned char)((val & 0x0000FF0000000000ull) >> 40);
	bufferP[offset++] = (unsigned char)((val & 0x000000FF00000000ull) >> 32);
	bufferP[offset++] = (unsigned char)((val & 0x00000000FF000000ull) >> 24);
	bufferP[offset++] = (unsigned char)((val & 0x0000000000FF0000ull) >> 16);
	bufferP[offset++] = (unsigned char)((val & 0x000000000000FF00ull) >> 8);
	bufferP[offset++] = (unsigned char)((val & 0x00000000000000FFull));
}

/**
 * \fn uint64_t RemoteProcedureCall::decode_uint64(unsigned char* bufferP, int& offset)
 * \brief return the uint64 value contained at &bufferP[offset] and increment
 *        the offset by the appropriate number of bytes.
 *
 * \param bufferP contains data serialized by SerializeCall or SerializeCallReturn.
 * \param offset is the index to the start of the value to decode in bufferP
 */
inline uint64_t RemoteProcedureCall::decode_uint64(unsigned char* bufferP, int& offset) {
	uint64_t ui64 = uint64_t(bufferP[offset++]) << 56;
	ui64 |= uint64_t(bufferP[offset++]) << 48;
	ui64 |= uint64_t(bufferP[offset++]) << 40;
	ui64 |= uint64_t(bufferP[offset++]) << 32;
	ui64 |= uint64_t(bufferP[offset++]) << 24;
	ui64 |= uint64_t(bufferP[offset++]) << 16;
	ui64 |= uint64_t(bufferP[offset++]) << 8;
	ui64 |= bufferP[offset++];

	return ui64;
}

/**
 * \fn int32_t RemoteProcedureCall::decode_int32(unsigned char* bufferP, int& offset)
 * \brief return the int32 value contained at &bufferP[offset] and increment
 *        the offset by the appropriate number of bytes.
 *
 * \param bufferP contains data serialized by SerializeCall or SerializeCallReturn.
 * \param offset is the index to the start of the value to decode in bufferP
 */
inline int32_t RemoteProcedureCall::decode_int32(unsigned char* bufferP, int& offset) {
	int32_t i32 = int32_t(bufferP[offset++]) << 24;
	i32 |= int32_t(bufferP[offset++]) << 16;
	i32 |= int32_t(bufferP[offset++]) << 8;
	i32 |= bufferP[offset++];

	return i32;
}

/**
 * \fn uint32_t RemoteProcedureCall::decode_uint32(unsigned char* bufferP, int& offset)
 * \brief return the uint32 value contained at &bufferP[offset] and increment
 *        the offset by the appropriate number of bytes.
 *
 * \param bufferP contains data serialized by SerializeCall or SerializeCallReturn.
 * \param offset is the index to the start of the value to decode in bufferP
 */
inline uint32_t RemoteProcedureCall::decode_uint32(unsigned char* bufferP, int& offset) {
	uint32_t ui32 = uint32_t(bufferP[offset++]) << 24;
	ui32 |= uint32_t(bufferP[offset++]) << 16;
	ui32 |= uint32_t(bufferP[offset++]) << 8;
	ui32 |= bufferP[offset++];

	return ui32;
}

/**
 * \fn int16_t RemoteProcedureCall::decode_int16(unsigned char* bufferP, int& offset)
 * \brief return the int16 value contained at &bufferP[offset] and increment
 *        the offset by the appropriate number of bytes.
 *
 * \param bufferP contains data serialized by SerializeCall or SerializeCallReturn.
 * \param offset is the index to the start of the value to decode in bufferP
 */
inline int16_t RemoteProcedureCall::decode_int16(unsigned char* bufferP, int& offset) {
	int16_t i16 = int16_t(bufferP[offset++]) << 8;
	i16 |= bufferP[offset++];

	return i16;
}

/**
 * \fn uint16_t RemoteProcedureCall::decode_uint16(unsigned char* bufferP, int& offset)
 * \brief return the uint16 value contained at &bufferP[offset] and increment
 *        the offset by the appropriate number of bytes.
 *
 * \param bufferP contains data serialized by SerializeCall or SerializeCallReturn.
 * \param offset is the index to the start of the value to decode in bufferP
 */
inline uint16_t RemoteProcedureCall::decode_uint16(unsigned char* bufferP, int& offset) {
	uint16_t ui16 = uint16_t(bufferP[offset++]) << 8;
	ui16 |= bufferP[offset++];

	return ui16;
}

/**
 * \fn vector<RemoteProcedureCall::Parameter*>* RemoteProcedureCall::DeserializeCall(const string& function)
 * \brief Deserializes a function call byte stream built by SerializeCall.
 *
 * \param asyncId is asynchronous call identifier, which will be returned with
 *		  the result to match it. It is O if the call is synchronous
 * \param function receives the name of the function to call
 * \param bufferP is the function call byte stream built by SerializeCall
 * 
 * \return (vector<RemoteProcedureCall::ParameterBase*>*)ReturnValue a valid ptr to a vector of ParameterBase if 
 *		   no error occured, NULL else (and an error set).
*/
ReturnValue<vector<RemoteProcedureCall::ParameterBase*>*, CommunicationErrors> RemoteProcedureCall::DeserializeCall(AsyncID& asyncId, string& function) {
	ReturnValue<vector<ParameterBase*>*, CommunicationErrors> r;
	vector<RemoteProcedureCall::ParameterBase*>*			  resultP;
	ParamType												  type;
	unsigned char											  b, *bufferP;
	unsigned long											  len;
	char													  c;
	uint16_t 												  ui16;
	uint32_t 												  ui32;
	uint64_t 												  ui64, ptr;
	double 													  d;
	int16_t 												  i16;
	int32_t 												  i32;
	int64_t 												  i64;
	int														  offset = 0;
	bool 													  done = false;
	string													  s;
	bool													  is_ptr = false;

#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 0, true, "RemoteProcedureCall::DeserializeCall(%s)", function.c_str());
#endif

	// an empty name is returned upon error
	function.clear();

	// wait for incoming call stream
	{
		unique_lock<mutex> lock(m_srv_receive_mutex);
		ReturnValue<unsigned char*, CommunicationErrors> rv;
		if ((rv = ReceivePacket(len)).IsError()) {
			r = {NULL, rv.GetError<CommunicationErrors::ErrorCode>()};
			return r;
		}

		bufferP = rv.GetResult();
	}

	resultP = new vector<RemoteProcedureCall::ParameterBase*>();

	// first get the asyncId
	if (bufferP[offset++] != (unsigned char)ASYNC_ID) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: asynchronous callee identifier name missing!" << endl;
		goto clean_up;
	}
	asyncId = (AsyncID)decode_uint64(bufferP, offset);
	asyncId = NTOHLL((uint64_t)asyncId);

#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "found asyncId : %lu", asyncId);
#endif

	// then, we must get the function name
	// which is of type STRING
	if (bufferP[offset++] != (unsigned char)STRING) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: function name missing!" << endl;
		goto clean_up;
	}

	// get the name len
	ui16 = decode_uint16(bufferP, offset);
	ui16 = ntohs(ui16);
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "decoded function len: %u", ui16);
#endif

	// get the name
	function = string((const char*)&bufferP[offset], ui16);
	offset += ui16;
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "decoded function %s", function.c_str());
#endif

	// next, we must get the return result address
	if (bufferP[offset++] != (unsigned char)UINT64) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: return result address missing!" << endl;
		goto clean_up;
	}

	// get the return result address
	ui64 = decode_uint64(bufferP, offset);
	ui64 = NTOHLL(ui64);
	resultP->push_back(new Parameter<uint64_t>(ui64));
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "decoded return result address: %lx", ui64);
#endif

	while (!done) {
		// get the parameter type
		type = (ParamType)bufferP[offset++];
#ifdef RPC_TRACES
		LogVText(RPC_MODULE, 6, true, "decoded parameter type %x", type);
#endif
		switch (type) {
			case PTR:
				// next parameter is a pointer
				is_ptr = true;
				break;

			case BYTE:
				if (is_ptr) {
					is_ptr = false;
					ptr = decode_uint64(bufferP, offset);
					ptr = NTOHLL(ptr);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded ptr %p", ptr);
#endif
					b = bufferP[offset++];
					resultP->push_back(new Parameter<unsigned char>(b, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded byte %u", b);
#endif
				} else {
					b = bufferP[offset++];
					resultP->push_back(new Parameter<unsigned char>(b));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded byte %u", b);
#endif
				}
				break;

			case CHAR:
				if (is_ptr) {
					is_ptr = false;
					ptr = decode_uint64(bufferP, offset);
					ptr = NTOHLL(ptr);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded ptr %p", ptr);
#endif
					c = bufferP[offset++];
					resultP->push_back(new Parameter<char>(c, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded byte %u", b);
#endif
				} else {
					c = bufferP[offset++];
					resultP->push_back(new Parameter<char>(c));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded char %c", c);
#endif
				}
				break;

			case INT16:
				if (is_ptr) {
					is_ptr = false;
					ptr = decode_uint64(bufferP, offset);
					ptr = NTOHLL(ptr);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded ptr %p", ptr);
#endif
					i16 = decode_int16(bufferP, offset);
					i16 = ntohs(i16);
					resultP->push_back(new Parameter<int16_t>(i16, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded int16_t %d", i16);
#endif
				} else {
					i16 = decode_int16(bufferP, offset);
					i16 = ntohs(i16);
					resultP->push_back(new Parameter<int16_t>(i16));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded int16_t %d", i16);
#endif
				}
				break;

			case UINT16:
				if (is_ptr) {
					is_ptr = false;
					ptr = decode_uint64(bufferP, offset);
					ptr = NTOHLL(ptr);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded ptr %p", ptr);
#endif
					ui16 = decode_uint16(bufferP, offset);
					ui16 = ntohs(ui16);
					resultP->push_back(new Parameter<uint16_t>(ui16, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded uint16_t %u", ui16);
#endif
				} else {
					ui16 = decode_uint16(bufferP, offset);
					ui16 = ntohs(ui16);
					resultP->push_back(new Parameter<uint16_t>(ui16));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded uint16_t %u", ui16);
#endif
				}
				break;

			case INT32:
				if (is_ptr) {
					is_ptr = false;
					ptr = decode_uint64(bufferP, offset);
					ptr = NTOHLL(ptr);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded ptr %p", ptr);
#endif
					i32 = decode_int32(bufferP, offset);
					i32 = ntohl(i32);
					resultP->push_back(new Parameter<int32_t>(i32, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded int32_t %d", i32);
#endif
				} else {
					i32 = decode_int32(bufferP, offset);
					i32 = ntohl(i32);
					resultP->push_back(new Parameter<int32_t>(i32));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded int32_t %d", i32);
#endif
				}
				break;

			case UINT32:
				if (is_ptr) {
					is_ptr = false;
					ptr = decode_uint64(bufferP, offset);
					ptr = NTOHLL(ptr);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded ptr %p", ptr);
#endif
					ui32 = decode_uint32(bufferP, offset);
					ui32 = ntohl(ui32);
					resultP->push_back(new Parameter<uint32_t>(ui32, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded uint32_t %u", ui32);
#endif
				} else {
					ui32 = decode_uint32(bufferP, offset);
					ui32 = ntohl(ui32);
					resultP->push_back(new Parameter<uint32_t>(ui32));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded uint32_t %u", ui32);
#endif
				}
				break;

			case INT64:
				if (is_ptr) {
					is_ptr = false;
					ptr = decode_uint64(bufferP, offset);
					ptr = NTOHLL(ptr);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded ptr %p", ptr);
#endif
					i64 = decode_int64(bufferP, offset);
					i64 = NTOHLL(i64);
					resultP->push_back(new Parameter<int64_t>(i64, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded int64_t %l", i64);
#endif
				} else {
					i64 = decode_int64(bufferP, offset);
					i64 = NTOHLL(i64);
					resultP->push_back(new Parameter<int64_t>(i64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded int64_t %l", i64);
#endif
				}
				break;

			case UINT64:
				if (is_ptr) {
					is_ptr = false;
					ptr = decode_uint64(bufferP, offset);
					ptr = NTOHLL(ptr);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded ptr %p", ptr);
#endif
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
					resultP->push_back(new Parameter<uint64_t>(ui64, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded uint64_t %lu", ui64);
#endif
				} else {
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
					resultP->push_back(new Parameter<uint64_t>(ui64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded uint64_t %lu", ui64);
#endif
				}
				break;

			case DOUBLE:
				if (is_ptr) {
					is_ptr = false;
					ptr = decode_uint64(bufferP, offset);
					ptr = NTOHLL(ptr);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded ptr %p", ptr);
#endif
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
					d = *(double*)&ui64;
					resultP->push_back(new Parameter<double>(d, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded double %f", d);
#endif
				} else {
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
					d = *(double*)&ui64;
					resultP->push_back(new Parameter<double>(d));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded double %f", d);
#endif
				}
				break;

			case STRING:
				if (is_ptr) {
					is_ptr = false;
					ptr = decode_uint64(bufferP, offset);
					ptr = NTOHLL(ptr);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded ptr %p", ptr);
#endif
					ui16 = decode_uint16(bufferP, offset);
					ui16 = ntohs(ui16);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded string len %u", ui16);
#endif
					s = string((const char*)&bufferP[offset], ui16);
					offset += ui16;
					resultP->push_back(new Parameter<string>(s, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded string %s", s.c_str());
#endif
				} else {
					ui16 = decode_uint16(bufferP, offset);
					ui16 = ntohs(ui16);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded string len %u", ui16);
#endif
					s = string((const char*)&bufferP[offset], ui16);
					offset += ui16;
					resultP->push_back(new Parameter<string>(s));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded string %s", s.c_str());
#endif
				}
				break;

			case END_OF_CALL:
				// marks the end of the parameters (and call stream)
				done = true;
#ifdef RPC_TRACES
				LogText(RPC_MODULE, 8, true, "decoded end of stream marker");
#endif
				break;

			default:
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: invalid parameter type!" << endl;
				done = true;
				break;
		}
	}

	free (bufferP);
	r = { resultP, CommunicationErrors::ErrorCode::None };
	return r;

clean_up:
	if (bufferP)
		free(bufferP);

	delete resultP;

	r = { NULL, CommunicationErrors::ErrorCode::ProtocolError };
	return r;
}

/**
 * \fn void RemoteProcedureCall::SerializeCallReturn(vector<Parameter*>* paramP, unsigned long retVal)
 * \brief Prepare serialized call return byte stream and send it over the associated link.
 *
 *        The stream format is:
 *				
 *	        	UINT64.<asyncId>.UINT64.<result_address>.UINT64.<return_value>.<parameters>.END_OF_CALL
 *  	      	result_address : address of the UINT64 to receive returnValue on the
 *                         		 caller side
 *				asyncId : UINT64 asynchronous call identifier
 *        		return_value : UINT64 value returned by callee
 *        		parameters := (PTR.<ptrValue>.<type>.<value>)*
 *        		ptrValue := address of the location to receive value on the caller
 *                     		 side
 *        		type := see ParamType in .h
 *        		value := output parameter value
 *        				 string values are streamed as uint16_t len followed by non asciiz
 *        				 string content
 *
 * \param asyncId is asynchronous call identifier, which will be returned with
 *				  the result to match it. It is O if the call is synchronous
 * \param paramP points to a parameter vector built by DeserializeCall
 * \param retVal is the server's procedure return value
 * 
 * \return (bool)ReturnValue is true if data could be sent, false else (and the
 *		   error code is set).
 */
ReturnValue<bool, CommunicationErrors> RemoteProcedureCall::SerializeCallReturn(AsyncID asyncId, shared_ptr<vector<ParameterBase*>> params, unsigned long retVal) {
	ReturnValue<bool, CommunicationErrors> r;
	vector<unsigned char> 				   serializedCall;
	ParamType							   type;
	unsigned char						   b;
	unsigned char* 						   p;
	char								   c;
	int16_t								   i16;
	uint16_t							   ui16;
	int32_t								   i32;
	uint32_t							   ui32;
	int64_t								   i64;
	uint64_t							   ui64;
	double								   d;
	string								   s;

#ifdef RPC_TRACES
	LogText(RPC_MODULE, 0, true, "RemoteProcedureCal::SerializeCallReturn(...)");
#endif

	serializedCall.push_back(ASYNC_ID);
	push_uint64(serializedCall, HTONLL((uint64_t)asyncId));

#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "pushed asyncId : %lu", (uint64_t)asyncId);
#endif

	serializedCall.push_back(UINT64);

	ui64 = ParameterSafeCast(uint64_t, params->at(0))->GetReference();
	push_uint64(serializedCall, HTONLL(ui64));
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "pushed return result address %lx", ui64);
#endif

	ui64 = (uint64_t)retVal;
	serializedCall.push_back(UINT64);
	push_uint64(serializedCall, HTONLL(ui64));
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "pushed return result: %lu", ui64);
#endif

	// push (only) ptr parameters back
	for (vector<ParameterBase*>::iterator i = params->begin(); i != params->end(); i++) {
		if (!(*i)->IsValidPointer())
			continue;

		// push ptr type
		serializedCall.push_back(PTR);
#ifdef RPC_TRACES
		LogText(RPC_MODULE, 6, true, "pushed ptr type");
#endif

		// push ptr
		ui64 = (*i)->GetCallerPointer();
		push_uint64(serializedCall, HTONLL(ui64));
#ifdef RPC_TRACES
		LogVText(RPC_MODULE, 6, true, "pushed parameter ptr %lx", ui64);
#endif

		// push type
		type = (*i)->GetType();
		serializedCall.push_back(type);
#ifdef RPC_TRACES
		LogVText(RPC_MODULE, 6, true, "pushed parameter type %x", type);
#endif

		// push value
		switch (type) {
			case BYTE:
				b = ParameterSafeCast(unsigned char, *i)->GetReference();
				serializedCall.push_back(b);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed byte parameter value %u", b);
#endif
				break;

			case CHAR:
				c = ParameterSafeCast(char, *i)->GetReference();
				serializedCall.push_back(c);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed char parameter value %c", c);
#endif
				break;

			case INT16:
				i16 = ParameterSafeCast(int16_t, *i)->GetReference();
				push_int16(serializedCall, htons(i16));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed int16 parameter value %d", i16);
#endif
				break;

			case UINT16:
				ui16 = ParameterSafeCast(uint16_t, *i)->GetReference();
				push_uint16(serializedCall, htons(ui16));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed uin16 parameter value %u", ui16);
#endif
				break;

			case INT32:
				i32 = ParameterSafeCast(int32_t, *i)->GetReference();
				push_int32(serializedCall, htonl(i32));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed int32 parameter value %d", i32);
#endif
				break;

			case UINT32:
				ui32 = ParameterSafeCast(uint32_t, *i)->GetReference();
				push_uint32(serializedCall, htonl(ui32));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed uint32 parameter value %u", ui32);
#endif
				break;

			case INT64:
				i64 = ParameterSafeCast(int64_t, *i)->GetReference();
				push_int64(serializedCall, HTONLL(i64));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed int64 parameter value %ld", i64);
#endif
				break;

			case UINT64:
				ui64 = ParameterSafeCast(uint64_t, *i)->GetReference();
				push_uint64(serializedCall, HTONLL(ui64));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed uint64 parameter value %lu", ui64);
#endif
				break;

			case DOUBLE:
				d = ParameterSafeCast(double, *i)->GetReference();
				ui64 = *(uint64_t*)&d;
				push_uint64(serializedCall, HTONLL(ui64));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed double parameter value %f", d);
#endif
				break;

			case STRING:
				s = ParameterSafeCast(string, *i)->GetReference();
				ui16 = (uint16_t)s.length();
				push_uint16(serializedCall, htons(ui16));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed string parameter value len %u", ui16);
#endif
				p = (unsigned char*)s.c_str();
				serializedCall.insert(serializedCall.end(), p, p+ui16);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed string parameter value %s", s.c_str());
#endif
				break;

			default:
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: invalid parameter type!" << endl;
				r = {false, CommunicationErrors::ErrorCode::BadArgument};
				return r;
		}
	}

	// end of call stream marker
	serializedCall.push_back(END_OF_CALL);
#ifdef RPC_TRACES
	LogText(RPC_MODULE, 4, true, "pushed end of stream marker");
#endif

	// send all the serialized call parameters over to the peer
	{
		unique_lock<mutex> lock(m_srv_send_mutex); 
		r = SendPacket(serializedCall.data(), (unsigned long)serializedCall.size()); 
	}
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "sent %ld bytes...", serializedCall.size());
#endif

	return r;
}

/**
 * \fn bool RemoteProcedureCall::DeserializeCallReturn(unsigned char* bufferP)
 * \brief Analyzes the given serialized call return byte stream and assigns
 *        the various 'out' parameters.
 *
 * \param asyncId is asynchronous call identifier, which will be returned with
 *				  the result to match it. It is O if the call is synchronous
 * \param bufferP points to a function call byte stream built by SerializeCallReturn
 * 
 * \return (bool)ReturnValue is true if data could be sent, false else (and the
 *		   error code is set).
 */
ReturnValue<bool, CommunicationErrors> RemoteProcedureCall::DeserializeCallReturn(AsyncID& asyncId, unsigned char* bufferP) {
	ReturnValue<bool, CommunicationErrors> r;
	unsigned long*						   resultP;
	int									   offset = 0;
	ParamType							   type;
	unsigned char						  b;
	char								  c;
	int16_t								  i16;
	uint16_t							  ui16;
	int32_t								  i32;
	uint32_t							  ui32;
	int64_t								  i64;
	uint64_t							  ui64, ptr;
	double 								  d;
	string								  s;
	bool								  done = false;
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 0, true, "RemoteProcedureCall::DeserializeCallReturn(%p)", bufferP);
#endif

	// get the asyncId
	if (bufferP[offset++] != (unsigned char)ASYNC_ID) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: missing callee asynchronous identifier!" << endl;
		r = {false, CommunicationErrors::ErrorCode::ProtocolError};
		return r;
	}
	asyncId = (AsyncID)decode_uint64(bufferP, offset);
	asyncId = NTOHLL((uint64_t)asyncId);

#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "found asyncId : %lu", asyncId);
#endif

	// read and decode the returned result address, value, optional output
	// parameters and end mark from the data stream
	// 
	// get the return result address
	if (bufferP[offset++] != (unsigned char)UINT64) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: missing return result address!" << endl;
		r = { false, CommunicationErrors::ErrorCode::MissingData };
		return r;
	}

	ui64 = decode_uint64(bufferP, offset);
	ui64 = NTOHLL(ui64);
	resultP = (unsigned long*)ui64;
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "decoded return result address: %lx", ui64);
#endif

	// get the result
	if (bufferP[offset++] != (unsigned char)UINT64) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: missing result value!" << endl;
		r = { false, CommunicationErrors::ErrorCode::MissingData };
		return r;
	}

	// set the result
	ui64 = decode_uint64(bufferP, offset);
	ui64 = NTOHLL(ui64);
	*resultP = (unsigned long)ui64;
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "decoded result: %lu", ui64);
#endif

	while (!done) {
		type = (ParamType)bufferP[offset++];
#ifdef RPC_TRACES
		LogVText(RPC_MODULE, 6, true, "decoded parameter type: %x", type);
#endif
		switch (type) {
			case PTR:
				// get ptr
				ui64 = decode_uint64(bufferP, offset);
				ptr = NTOHLL(ui64);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 6, true, "decoded parameter ptr: %p", ptr);
#endif
				break;

			case BYTE:
				b = (unsigned char)bufferP[offset++];
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "decoded parameter byte value: %u", b);
#endif
				*(unsigned char*)ptr = b;
				break;

			case CHAR:
				c = (char)bufferP[offset++];
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "decoded parameter char value: %c", c);
#endif
				*(char*)ptr = c;
				break;

			case INT16:
				i16 = decode_int16(bufferP, offset);
				i16 = ntohs(i16);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "decoded parameter int16 value: %d", i16);
#endif
				*(int16_t*)ptr = i16;
				break;

			case UINT16:
				ui16 = decode_uint16(bufferP, offset);
				ui16 = ntohs(ui16);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "decoded parameter uint16 value: %u", ui16);
#endif
				*(uint16_t*)ptr = ui16;
				break;

			case INT32:
				i32 = decode_int32(bufferP, offset);
				i32 = ntohl(i32);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "decoded parameter int32 value: %d", i32);
#endif
				*(int32_t*)ptr = i32;
				break;

			case UINT32:
				ui32 = decode_uint32(bufferP, offset);
				ui32 = ntohl(ui32);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "decoded parameter uint32 value: %u", ui32);
#endif
				*(uint32_t*)ptr = ui32;
				break;

			case INT64:
				i64 = decode_int64(bufferP, offset);
				i64 = NTOHLL(i64);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "decoded parameter int64 value: %l", i64);
#endif
				*(int64_t*)ptr = i64;
				break;

			case UINT64:
				ui64 = decode_uint64(bufferP, offset);
				ui64 = NTOHLL(ui64);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "decoded parameter uint64 value: %lu", ui64);
#endif
				*(uint64_t*)ptr = ui64;
				break;

			case DOUBLE:
				ui64 = decode_uint64(bufferP, offset);
				ui64 = NTOHLL(ui64);
				d = *(double*)&ui64;
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "decoded parameter double value: %f", d);
#endif
				*(double*)ptr = d;
				break;

			case STRING:
				ui16 = decode_uint16(bufferP, offset);
				ui16 = ntohs(ui16);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "decoded parameter string len: %u", ui16);
#endif
				s = string((const char*)&bufferP[offset], ui16);
				offset += ui16;
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "decoded parameter string: %s", s.c_str());
#endif
				*(string*)ptr = s;
				break;

			case END_OF_CALL:
#ifdef RPC_TRACES
				LogText(RPC_MODULE, 8, true, "decoded end of stream marker");
#endif
				done = true;
				break;

			default:
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: unknown parameter type!" << endl;
				done = true;
				break;
		}
	}

	r = { false, CommunicationErrors::ErrorCode::None };
	return r;
}

