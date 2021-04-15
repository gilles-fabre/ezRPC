#include <stdio.h>
#include <stdarg.h>
#include <Link.h>
#include <iostream>
#include <log2reporter.h>

#include "RemoteProcedureCall.h"

using namespace std;

bool RemoteProcedureCall::SendPacket(unsigned char *bufferP, unsigned long data_len) {
	uint64_t 		len = data_len;
	unsigned char 	buffer[sizeof(uint64_t)];

	if (len == 0 || !bufferP)
		return false;

	// encode the packet length
	int offset = 0;
	encode_uint64(HTONLL(len), buffer, offset);

	// send the packet len
	m_linkP->Send(buffer, (unsigned long)sizeof(len));

	// send the whole call stream
	m_linkP->Send(bufferP, data_len);

	return true;
}

unsigned char *RemoteProcedureCall::ReceivePacket(unsigned long &data_len) {
	uint64_t 		len;
	unsigned char 	buffer[sizeof(uint64_t)];
	unsigned char 	*bufferP;

	// retrieve the packet len
	m_linkP->Receive(buffer, (unsigned long)sizeof(len));

	int offset = 0;
	len = decode_uint64(buffer, offset);
	if ((data_len = (unsigned long)NTOHLL(len)) == 0)
		return NULL;

	// allocate the necessary bufferP
	if (!(bufferP = (unsigned char *)malloc(data_len)))
		return NULL;

	// wait and read the whole call stream
	if (!m_linkP->Receive(bufferP, (unsigned long)data_len)) {
		free(bufferP);  // garbage received before connection dropped
		bufferP = NULL;
	}

	return bufferP;
}

/**
 * \fn void RemoteProcedureCall::push_int64(vector<unsigned char> &v, int64_t val)
 * \brief pushes an int64 into the given serialization vector.
 *
 * \param v is a byte vector containing the serialized call stream
 * \param val is the data to serialize into the vector v
 */
inline void RemoteProcedureCall::push_int64(vector<unsigned char> &v, int64_t val) {
	v.push_back((val & 0xFF00000000000000ull) >> 56);
	v.push_back((val & 0x00FF000000000000ull) >> 48);
	v.push_back((val & 0x0000FF0000000000ull) >> 40);
	v.push_back((val & 0x000000FF00000000ull) >> 32);
	v.push_back((val & 0x00000000FF000000ull) >> 24);
	v.push_back((val & 0x0000000000FF0000ull) >> 16);
	v.push_back((val & 0x000000000000FF00ull) >> 8);
	v.push_back((val & 0x00000000000000FFull));
}

/**
 * \fn void RemoteProcedureCall::push_uint64(vector<unsigned char> &v, uint64_t val)
 * \brief pushes a uint64 into the given serialization vector.
 *
 * \param v is a byte vector containing the serialized call stream
 * \param val is the data to serialize into the vector v
 */
inline void RemoteProcedureCall::push_uint64(vector<unsigned char> &v, uint64_t val) {
	v.push_back((val & 0xFF00000000000000ull) >> 56);
	v.push_back((val & 0x00FF000000000000ull) >> 48);
	v.push_back((val & 0x0000FF0000000000ull) >> 40);
	v.push_back((val & 0x000000FF00000000ull) >> 32);
	v.push_back((val & 0x00000000FF000000ull) >> 24);
	v.push_back((val & 0x0000000000FF0000ull) >> 16);
	v.push_back((val & 0x000000000000FF00ull) >> 8);
	v.push_back((val & 0x00000000000000FFull));
}

/**
 * \fn void RemoteProcedureCall::push_int32(vector<unsigned char> &v, int32_t val)
 * \brief pushes an int32 into the given serialization vector.
 *
 * \param v is a byte vector containing the serialized call stream
 * \param val is the data to serialize into the vector v
 */
inline void RemoteProcedureCall::push_int32(vector<unsigned char> &v, int32_t val) {
	v.push_back((val & 0xFF000000) >> 24);
	v.push_back((val & 0x00FF0000) >> 16);
	v.push_back((val & 0x0000FF00) >> 8);
	v.push_back((val & 0x000000FF));
}

/**
 * \fn void RemoteProcedureCall::push_uint32(vector<unsigned char> &v, uint32_t val)
 * \brief pushes a uint32 into the given serialization vector.
 *
 * \param v is a byte vector containing the serialized call stream
 * \param val is the data to serialize into the vector v
 */
inline void RemoteProcedureCall::push_uint32(vector<unsigned char> &v, uint32_t val) {
	v.push_back((val & 0xFF000000) >> 24);
	v.push_back((val & 0x00FF0000) >> 16);
	v.push_back((val & 0x0000FF00) >> 8);
	v.push_back((val & 0x000000FF));
}

/**
 * \fn void RemoteProcedureCall::push_int16(vector<unsigned char> &v, int16_t val)
 * \brief pushes an int16 into the given serialization vector.
 *
 * \param v is a byte vector containing the serialized call stream
 * \param val is the data to serialize into the vector v
 */
inline void RemoteProcedureCall::push_int16(vector<unsigned char> &v, int16_t val) {
	v.push_back((val & 0xFF00) >> 8);
	v.push_back(val & 0x00FF);
}

/**
 * \fn void RemoteProcedureCall::push_uint16(vector<unsigned char> &v, uint16_t val)
 * \brief pushes a uint16 into the given serialization vector.
 *
 * \param v is a byte vector containing the serialized call stream
 * \param val is the data to serialize into the vector v
 */
inline void RemoteProcedureCall::push_uint16(vector<unsigned char> &v, uint16_t val) {
	v.push_back((val & 0xFF00) >> 8);
	v.push_back(val & 0x00FF);
}

/**
 * \fn unsigned long RemoteProcedureCall::SerializeCall(string &func_name, ...)
 * \brief Serializes the given function call and sends the resulting byte stream
 *        over the associated link.
 *
 *        The stream format is:
 *	        	STRING.<function_name>.UINT64.<result_address>.<parameters>.END_OF_CALL
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
 * \param func_name is the name of the function to call
 * \param ... is var list of parameters with the following format
 * 		  (<type><value>)|(PTR<pointer_to_value>). The last argument must be
 * 		  'END_OF_CALL'.
 *        STRING values MUST be passed as string pointers (std::string*)
 * \return the unsigned long result passed to SerializedCallReturn
 */
inline unsigned long RemoteProcedureCall::SerializeCall(const string &func_name, ...) {
	unsigned long result = 0;
	va_list 	  vl;

	va_start(vl, func_name);
	result = SerializeCall(func_name, vl);
	va_end(vl);

	return result;
}

unsigned long RemoteProcedureCall::SerializeCall(const string &func_name, va_list vl) {
	vector<unsigned char> 	serialized_call;
	unsigned long			result = 0;
	unsigned int 			len;
	string					s;
	unsigned char			byte;
	char					c;
	int16_t					i16;
	uint16_t				ui16;
	int32_t					i32;
	uint32_t				ui32;
	int64_t					i64;
	uint64_t				ui64;
	bool 					is_ptr = false;
	void					*ptr;
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 0, true, "RemoteProcedureCall::SerializeCall(%s)", func_name.c_str());
#endif

	// first pass the function name
	serialized_call.push_back(STRING);
	len = (unsigned int)func_name.length();
	push_uint16(serialized_call, htons(len));
	for (int i = 0; i < len; i++)
		serialized_call.push_back(func_name[i]);
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "pushed func_name %s", func_name.c_str());
#endif

	// then add the return result address
	serialized_call.push_back(UINT64);
	push_uint64(serialized_call, HTONLL((uint64_t)&result));
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "pushed return address %p", &result);
#endif

	// and all the given parameters
	for (;;) {
		char type = (char)va_arg(vl, int);

		// first push the parameter type in the call stream vector
		serialized_call.push_back(type);
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
					ptr = (void *)va_arg(vl, unsigned char *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					serialized_call.push_back(ptr ? *(unsigned char *)ptr : 0);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter byte ptr %p", ptr);
#endif
				} else {
					// a byte
					byte = (unsigned char)va_arg(vl, int);
					serialized_call.push_back(byte);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter byte %u", byte);
#endif
				}
				break;

			case CHAR:
				if (is_ptr) {
					is_ptr = false;

					// a char ptr
					ptr = (void *)va_arg(vl, char *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					serialized_call.push_back(ptr ? *(char *)ptr : 0);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter char ptr %p", ptr);
#endif
				} else {
					// a char
					c = (char)va_arg(vl, int);
					serialized_call.push_back(c);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter char %c", c);
#endif
				}
				break;

			case INT16:
				if (is_ptr) {
					is_ptr = false;

					// a short integer ptr
					ptr = (void *)va_arg(vl, int16_t *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					i16 = ptr ? *(int16_t *)ptr : 0;
					push_int16(serialized_call, ntohs(i16));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter int16 ptr %p", ptr);
#endif
				} else {
					// a short integer
					i16 = (int16_t)va_arg(vl, int);
					push_int16(serialized_call, htons(i16));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter int16_t %ud", i16);
#endif
				}
				break;

			case UINT16:
				if (is_ptr) {
					is_ptr = false;

					// an unsigned short ptr
					ptr = (void *)va_arg(vl, uint16_t *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					ui16 = ptr ? *(uint16_t *)ptr : 0;
					push_uint16(serialized_call, ntohs(ui16));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter uint16_t ptr %p", ptr);
#endif
				} else {
					// an unsigned short integer
					ui16 = (uint16_t)va_arg(vl, int);
					push_uint16(serialized_call, htons(ui16));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter uint16_t %u", ui16);
#endif
				}
				break;

			case INT32:
				if (is_ptr) {
					is_ptr = false;

					// a long integer ptr
					ptr = (void *)va_arg(vl, int32_t *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					i32 = ptr ? *(int32_t *)ptr : 0;
					push_int32(serialized_call, ntohl(i32));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter int32_t ptr %p", ptr);
#endif
				} else {
					// a long integer
					i32 = (int32_t)va_arg(vl, int32_t);
					push_int32(serialized_call, htonl(i32));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter int32_t %d", i32);
#endif
				}
				break;

			case UINT32:
				if (is_ptr) {
					is_ptr = false;

					// an unsigned long ptr
					ptr = (void *)va_arg(vl, uint32_t *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					ui32 = ptr ? *(uint32_t *)ptr : 0;
					push_uint32(serialized_call, ntohl(ui32));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter uint32_t ptr %p", ptr);
#endif
				} else {
					// an unsigned long integer
					ui32 = (uint32_t)va_arg(vl, uint32_t);
					push_uint32(serialized_call, htonl(ui32));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter uint32_t %u", ui32);
#endif
				}
				break;

			case INT64:
				if (is_ptr) {
					is_ptr = false;

					// a long long ptr
					ptr = (void *)va_arg(vl, int64_t *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					i64 = ptr ? *(int64_t *)ptr : 0;
					push_int64(serialized_call, NTOHLL(i64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter int64_t ptr %p", ptr);
#endif
				} else {
					// a long long integer
					i64 = va_arg(vl, int64_t);
					push_int64(serialized_call, HTONLL(i64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter int64_t %ld", i64);
#endif
				}
				break;

			case UINT64:
				if (is_ptr) {
					is_ptr = false;

					// an unsigned long long ptr
					ptr = (void *)va_arg(vl, uint64_t *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					ui64 = ptr ? *(uint64_t *)ptr : 0;
					push_uint64(serialized_call, NTOHLL(ui64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter uint64_t ptr %p", ptr);
#endif
				} else {
					// an unsigned long long integer
					ui64 = va_arg(vl, uint64_t);
					push_uint64(serialized_call, HTONLL(ui64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter uint64_t %lu", ui64);
#endif
				}
				break;

			case DOUBLE:
				if (is_ptr) {
					is_ptr = false;

					// a double ptr
					ptr = (void *)va_arg(vl, double *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					ui64 = ptr ? *(uint64_t *)ptr : 0;
					push_uint64(serialized_call, NTOHLL(ui64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter double ptr %p", ptr);
#endif
				} else {
					// a double
					double d = (double)va_arg(vl, double);
					ui64 = *(uint64_t *)&d;
					push_uint64(serialized_call, HTONLL(ui64));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter double %f", d);
#endif
				}
				break;

			case STRING:
				if (is_ptr) {
					is_ptr = false;

					// a string ptr
					ptr = (void *)va_arg(vl, string *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter string %p", ptr);
#endif
					// string
					s = ptr ? *(string *)ptr : "";
					len = (unsigned int)s.length();
					push_uint16(serialized_call, htons(len));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter string len %u", len);
#endif
					for (int i = 0; i < len; i++)
						serialized_call.push_back(s[i]);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter string %s", s.c_str());
#endif
				} else {
					// string
					s = *va_arg(vl, string *);
					len = (unsigned int)s.length();
					push_uint16(serialized_call, htons(len));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter string len %u", len);
#endif
					for (int i = 0; i < len; i++)
						serialized_call.push_back(s[i]);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "pushed parameter string %s", s.c_str());
#endif
				}
				break;
		}
	}

	// send all the serialized call parameters over to the peer
	unsigned long buff_len = serialized_call.size();
	SendPacket(serialized_call.data(), buff_len);

#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "sent %lu bytes. Will now wait for reply...", buff_len);
#endif

	// wait for reply
	unsigned char *bufferP;
	if ((bufferP = ReceivePacket(buff_len))) {
		DeserializeCallReturn(bufferP);

		free(bufferP);
	}

	return (unsigned long)result;
}

/**
 * \fn int64_t RemoteProcedureCall::decode_int64(unsigned char *bufferP, int &offset)
 * \brief return the int64 value contained at &bufferP[offset] and increment
 *        the offset by the appropriate number of bytes.
 *
 * \param bufferP contains data serialized by SerializeCall or SerializeCallReturn.
 * \param offset is the index to the start of the value to decode in bufferP
 */
inline int64_t RemoteProcedureCall::decode_int64(unsigned char *bufferP, int &offset) {
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

inline void RemoteProcedureCall::encode_uint64(uint64_t val, unsigned char *bufferP, int &offset) {
	bufferP[offset++] = ((val & 0xFF00000000000000ull) >> 56);
	bufferP[offset++] = ((val & 0x00FF000000000000ull) >> 48);
	bufferP[offset++] = ((val & 0x0000FF0000000000ull) >> 40);
	bufferP[offset++] = ((val & 0x000000FF00000000ull) >> 32);
	bufferP[offset++] = ((val & 0x00000000FF000000ull) >> 24);
	bufferP[offset++] = ((val & 0x0000000000FF0000ull) >> 16);
	bufferP[offset++] = ((val & 0x000000000000FF00ull) >> 8);
	bufferP[offset++] = ((val & 0x00000000000000FFull));
}

/**
 * \fn uint64_t RemoteProcedureCall::decode_uint64(unsigned char *bufferP, int &offset)
 * \brief return the uint64 value contained at &bufferP[offset] and increment
 *        the offset by the appropriate number of bytes.
 *
 * \param bufferP contains data serialized by SerializeCall or SerializeCallReturn.
 * \param offset is the index to the start of the value to decode in bufferP
 */
inline uint64_t RemoteProcedureCall::decode_uint64(unsigned char *bufferP, int &offset) {
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
 * \fn int32_t RemoteProcedureCall::decode_int32(unsigned char *bufferP, int &offset)
 * \brief return the int32 value contained at &bufferP[offset] and increment
 *        the offset by the appropriate number of bytes.
 *
 * \param bufferP contains data serialized by SerializeCall or SerializeCallReturn.
 * \param offset is the index to the start of the value to decode in bufferP
 */
inline int32_t RemoteProcedureCall::decode_int32(unsigned char *bufferP, int &offset) {
	int32_t i32 = int32_t(bufferP[offset++]) << 24;
	i32 |= int32_t(bufferP[offset++]) << 16;
	i32 |= int32_t(bufferP[offset++]) << 8;
	i32 |= bufferP[offset++];

	return i32;
}

/**
 * \fn uint32_t RemoteProcedureCall::decode_uint32(unsigned char *bufferP, int &offset)
 * \brief return the uint32 value contained at &bufferP[offset] and increment
 *        the offset by the appropriate number of bytes.
 *
 * \param bufferP contains data serialized by SerializeCall or SerializeCallReturn.
 * \param offset is the index to the start of the value to decode in bufferP
 */
inline uint32_t RemoteProcedureCall::decode_uint32(unsigned char *bufferP, int &offset) {
	uint32_t ui32 = uint32_t(bufferP[offset++]) << 24;
	ui32 |= uint32_t(bufferP[offset++]) << 16;
	ui32 |= uint32_t(bufferP[offset++]) << 8;
	ui32 |= bufferP[offset++];

	return ui32;
}

/**
 * \fn int16_t RemoteProcedureCall::decode_int16(unsigned char *bufferP, int &offset)
 * \brief return the int16 value contained at &bufferP[offset] and increment
 *        the offset by the appropriate number of bytes.
 *
 * \param bufferP contains data serialized by SerializeCall or SerializeCallReturn.
 * \param offset is the index to the start of the value to decode in bufferP
 */
inline int16_t RemoteProcedureCall::decode_int16(unsigned char *bufferP, int &offset) {
	int16_t i16 = int16_t(bufferP[offset++]) << 8;
	i16 |= bufferP[offset++];

	return i16;
}

/**
 * \fn uint16_t RemoteProcedureCall::decode_uint16(unsigned char *bufferP, int &offset)
 * \brief return the uint16 value contained at &bufferP[offset] and increment
 *        the offset by the appropriate number of bytes.
 *
 * \param bufferP contains data serialized by SerializeCall or SerializeCallReturn.
 * \param offset is the index to the start of the value to decode in bufferP
 */
inline uint16_t RemoteProcedureCall::decode_uint16(unsigned char *bufferP, int &offset) {
	uint16_t ui16 = uint16_t(bufferP[offset++]) << 8;
	ui16 |= bufferP[offset++];

	return ui16;
}

/**
 * \fn vector<RemoteProcedureCall::Parameter *> *RemoteProcedureCall::DeserializeCall(unsigned char *bufferP, string &func_name)
 * \brief Deserializes a function call byte stream built by SerializeCall.
 *
 * \param func_name receives the name of the function to call
 * \param bufferP is the function call byte stream built by SerializeCall
 * \return a vector of parameters associated with the function call
 */
vector<RemoteProcedureCall::Parameter *> *RemoteProcedureCall::DeserializeCall(string &func_name) {
	vector<RemoteProcedureCall::Parameter *> *resultP;
	unsigned char	b, type, *bufferP;
	unsigned long	len;
	char			c;
	uint16_t 		ui16;
	uint32_t 		ui32;
	uint64_t 		ui64, ptr;
	double 			d;
	int16_t 		i16;
	int32_t 		i32;
	int64_t 		i64;
	int				offset = 0;
	bool 			done = false;
	string			s;
	bool			is_ptr = false;
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 0, true, "RemoteProcedureCall::DeserializeCall(%s)", func_name.c_str());
#endif

	// an empty name is returned upon error
	func_name.clear();

	// wait for incoming call stream
	if (!(bufferP = ReceivePacket(len)))
		return NULL;

	resultP = new vector<RemoteProcedureCall::Parameter *>();

	// first, we must get the function name
	// which is of type STRING
	if (bufferP[offset++] != (unsigned char)STRING) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: function name missing!" << endl;
		goto clean_up;
	}

	// get the name len
	ui16 = decode_uint16(bufferP, offset);
	ui16 = ntohs(ui16);
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "decoded func_name len: %u", ui16);
#endif

	// get the name
	for (int i = 0; i < ui16; i ++)
		func_name += bufferP[offset++];
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "decoded func_name %s", func_name.c_str());
#endif

	// next, we must get the return result address
	if (bufferP[offset++] != (unsigned char)UINT64) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: return result address missing!" << endl;
		goto clean_up;
	}

	// get the return result address
	ui64 = decode_uint64(bufferP, offset);
	ui64 = NTOHLL(ui64);
	resultP->push_back(new Parameter(ui64));
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "decoded return result address: %lx", ui64);
#endif

	while (!done) {
		// get the parameter type
		type = bufferP[offset++];
#ifdef RPC_TRACES
		LogVText(RPC_MODULE, 6, true, "decoded parameter type %x", type);
#endif
		switch ((ParamType)type) {
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
					resultP->push_back(new Parameter(b, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded byte %u", b);
#endif
				} else {
					b = bufferP[offset++];
					resultP->push_back(new Parameter(b));
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
					resultP->push_back(new Parameter(c, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded byte %u", b);
#endif
				} else {
					c = bufferP[offset++];
					resultP->push_back(new Parameter(c));
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
					LogVText(RPC_MODULE, 4, true, "decoded ptr %p", ptr);
#endif
					i16 = decode_int16(bufferP, offset);
					i16 = ntohs(i16);
					resultP->push_back(new Parameter(i16, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded int16_t %d", i16);
#endif
				} else {
					i16 = decode_int16(bufferP, offset);
					i16 = ntohs(i16);
					resultP->push_back(new Parameter(i16));
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
					LogVText(RPC_MODULE, 4, true, "decoded ptr %p", ptr);
#endif
					ui16 = decode_uint16(bufferP, offset);
					ui16 = ntohs(ui16);
					resultP->push_back(new Parameter(ui16, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded uint16_t %u", ui16);
#endif
				} else {
					ui16 = decode_uint16(bufferP, offset);
					ui16 = ntohs(ui16);
					resultP->push_back(new Parameter(ui16));
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
					LogVText(RPC_MODULE, 4, true, "decoded ptr %p", ptr);
#endif
					i32 = decode_int32(bufferP, offset);
					i32 = ntohl(i32);
					resultP->push_back(new Parameter(i32, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded int32_t %d", i32);
#endif
				} else {
					i32 = decode_int32(bufferP, offset);
					i32 = ntohl(i32);
					resultP->push_back(new Parameter(i32));
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
					LogVText(RPC_MODULE, 4, true, "decoded ptr %p", ptr);
#endif
					ui32 = decode_uint32(bufferP, offset);
					ui32 = ntohl(ui32);
					resultP->push_back(new Parameter(ui32, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded uint32_t %u", ui32);
#endif
				} else {
					ui32 = decode_uint32(bufferP, offset);
					ui32 = ntohl(ui32);
					resultP->push_back(new Parameter(ui32));
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
					LogVText(RPC_MODULE, 4, true, "decoded ptr %p", ptr);
#endif
					i64 = decode_int64(bufferP, offset);
					i64 = NTOHLL(i64);
					resultP->push_back(new Parameter(i64, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded int64_t %l", i64);
#endif
				} else {
					i64 = decode_int64(bufferP, offset);
					i64 = NTOHLL(i64);
					resultP->push_back(new Parameter(i64));
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
					LogVText(RPC_MODULE, 4, true, "decoded ptr %p", ptr);
#endif
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
					resultP->push_back(new Parameter(ui64, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded uint64_t %lu", ui64);
#endif
				} else {
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
					resultP->push_back(new Parameter(ui64));
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
					LogVText(RPC_MODULE, 4, true, "decoded ptr %p", ptr);
#endif
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
					d = *(double *)&ui64;
					resultP->push_back(new Parameter(d, ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded double %f", d);
#endif
				} else {
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
					d = *(double *)&ui64;
					resultP->push_back(new Parameter(d));
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
					LogVText(RPC_MODULE, 4, true, "decoded ptr %p", ptr);
#endif
					ui16 = decode_uint16(bufferP, offset);
					ui16 = ntohs(ui16);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded string len %u", ui16);
#endif
					s.clear();
					for (int i = 0; i < ui16; i ++)
						s += bufferP[offset++];
					resultP->push_back(new Parameter(s.c_str(), ptr));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded string %s", s.c_str());
#endif
				} else {
					ui16 = decode_uint16(bufferP, offset);
					ui16 = ntohs(ui16);
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded string len %u", ui16);
#endif
					s.clear();
					for (int i = 0; i < ui16; i ++)
						s += bufferP[offset++];
					resultP->push_back(new Parameter(s.c_str()));
#ifdef RPC_TRACES
					LogVText(RPC_MODULE, 8, true, "decoded string %s", s.c_str());
#endif
				}
				break;

			case END_OF_CALL:
				// marks the end of the parameters (and call stream)
				done = true;
#ifdef RPC_TRACES
				LogText(RPC_MODULE, 4, true, "decoded end of stream marker");
#endif
				break;

			default:
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: invalid parameter type!" << endl;
				done = true;
				break;
		}
	}

	free (bufferP);
	return resultP;

clean_up:
	if (bufferP)
		free(bufferP);

	delete resultP;

	return NULL;
}

/**
 * \fn void RemoteProcedureCall::SerializeCallReturn(vector<Parameter *>* paramP, unsigned long ret_val)
 * \brief Prepare serialized call return byte stream and send it over the associated link.
 *
 *        The stream format is:
 *	        	UINT64.<result_address>.UINT64.<return_value>.<parameters>.END_OF_CALL
 *  	      	result_address : address of the UINT64 to receive return_value on the
 *                         		 caller side
 *        		return_value : UINT64 value returned by callee
 *        		parameters := (PTR.<ptr_value>.<type>.<value>)*
 *        		ptr_value := address of the location to receive value on the caller
 *                     		 side
 *        		type := see ParamType in .h
 *        		value := output parameter value
 *        				 string values are streamed as uint16_t len followed by non asciiz
 *        				 string content
 *
 * \param paramP points to a parameter vector built by DeserializeCall
 * \param result is the unsigned long function call result
 */
void RemoteProcedureCall::SerializeCallReturn(vector<Parameter *>* paramP, unsigned long ret_val) {
	vector<unsigned char> 	serialized_call;
	unsigned char			b, type;
	char					c;
	int16_t					i16;
	uint16_t				ui16;
	int32_t					i32;
	uint32_t				ui32;
	int64_t					i64;
	uint64_t				ui64;
	double					d;
	string					s;
#ifdef RPC_TRACES
	LogText(RPC_MODULE, 0, true, "RemoteProcedureCal::SerializeCallReturn(...)");
#endif

	serialized_call.push_back(UINT64);

	ui64 = paramP->at(0)->GetUInt64Value();
	push_uint64(serialized_call, HTONLL(ui64));
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 0, true, "pushed return result address %lx", ui64);
#endif

	ui64 = (uint64_t)ret_val;
	serialized_call.push_back(UINT64);
	push_uint64(serialized_call, HTONLL(ui64));
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 0, true, "pushed return result: %lu", ui64);
#endif

	// push all ptr parameters back
	for (vector<Parameter *>::iterator i = paramP->begin(); i != paramP->end(); i++) {
		if (!(*i)->IsValidPointer())
			continue;

		// push ptr type
		serialized_call.push_back(PTR);
#ifdef RPC_TRACES
		LogText(RPC_MODULE, 4, true, "pushed ptr type");
#endif

		// push ptr
		ui64 = (*i)->GetCallerPointer();
		push_uint64(serialized_call, HTONLL(ui64));
#ifdef RPC_TRACES
		LogVText(RPC_MODULE, 8, true, "pushed parameter ptr %lx", ui64);
#endif

		// push type
		type = (unsigned char)(*i)->GetType();
		serialized_call.push_back(type);
#ifdef RPC_TRACES
		LogVText(RPC_MODULE, 8, true, "pushed parameter type %x", type);
#endif

		// push value
		switch (type) {
			case BYTE:
				b = (*i)->GetByteValue();
				serialized_call.push_back(b);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed byte parameter value %u", b);
#endif
				break;

			case CHAR:
				c = (*i)->GetCharValue();
				serialized_call.push_back(c);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed char parameter value %c", c);
#endif
				break;

			case INT16:
				i16 = (*i)->GetInt16Value();
				push_int16(serialized_call, htons(i16));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed int16 parameter value %d", i16);
#endif
				break;

			case UINT16:
				ui16 = (*i)->GetUInt16Value();
				push_uint16(serialized_call, htons(ui16));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed uin16 parameter value %u", ui16);
#endif
				break;

			case INT32:
				i32 = (*i)->GetInt32Value();
				push_int32(serialized_call, htonl(i32));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed int32 parameter value %d", i32);
#endif
				break;

			case UINT32:
				ui32 = (*i)->GetUInt32Value();
				push_uint32(serialized_call, htonl(ui32));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed uint32 parameter value %u", ui32);
#endif
				break;

			case INT64:
				i64 = (*i)->GetInt64Value();
				push_int64(serialized_call, HTONLL(i64));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed int64 parameter value %ld", i64);
#endif
				break;

			case UINT64:
				ui64 = (*i)->GetUInt64Value();
				push_uint64(serialized_call, HTONLL(ui64));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed uint64 parameter value %lu", ui64);
#endif
				break;

			case DOUBLE:
				d = (*i)->GetDoubleValue();
				ui64 = *(uint64_t *)&d;
				push_uint64(serialized_call, HTONLL(ui64));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed double parameter value %f", d);
#endif
				break;

			case STRING:
				s = (*i)->GetStringValue();
				ui16 = s.length();
				push_uint16(serialized_call, htons(ui16));
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed string parameter value len %u", ui16);
#endif

				for (int i = 0; i < ui16; i++)
					serialized_call.push_back(s[i]);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 8, true, "pushed string parameter value %s", s.c_str());
#endif
				break;

			default:
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: invalid parameter type!" << endl;
				break;
		}
	}

	// end of call stream marker
	serialized_call.push_back(END_OF_CALL);
#ifdef RPC_TRACES
	LogText(RPC_MODULE, 4, true, "pushed end of stream marker");
#endif

	// send all the serialized call parameters over to the peer
	SendPacket(serialized_call.data(), serialized_call.size());
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "sent %ld bytes...", serialized_call.size());
#endif
}

/**
 * \fn bool RemoteProcedureCall::DeserializeCallReturn(unsigned char *bufferP)
 * \brief Analyzes the given serialized call return byte stream and assigns
 *        the various 'out' parameters.
 *
 * \param bufferP points to a function call byte stream built by SerializeCallReturn
 */
bool RemoteProcedureCall::DeserializeCallReturn(unsigned char *bufferP) {
	unsigned long 	*resultP;
	int				offset = 0;
	unsigned char	b, type;
	char			c;
	int16_t			i16;
	uint16_t		ui16;
	int32_t			i32;
	uint32_t		ui32;
	int64_t			i64;
	uint64_t		ui64, ptr;
	double 			d;
	string			s;
	bool			done = false;
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 0, true, "RemoteProcedureCall::DeserializeCallReturn(%p)", bufferP);
#endif

	// read and decode the returned result address, value, optional output
	// parameters and end mark from the data stream

	// get the return result address
	if (bufferP[offset++] != (unsigned char)UINT64) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: missing return result address!" << endl;
		return false;
	}

	ui64 = decode_uint64(bufferP, offset);
	ui64 = NTOHLL(ui64);
	resultP = (unsigned long *)ui64;
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "decoded return result address: %lx", ui64);
#endif

	// get the result
	if (bufferP[offset++] != (unsigned char)UINT64) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: missing result value!" << endl;
		return false;
	}

	// set the result
	ui64 = decode_uint64(bufferP, offset);
	ui64 = NTOHLL(ui64);
	*resultP = (unsigned long)ui64;
#ifdef RPC_TRACES
	LogVText(RPC_MODULE, 4, true, "decoded result: %lu", ui64);
#endif

	while (!done) {
		type = bufferP[offset++];
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
				LogVText(RPC_MODULE, 6, true, "decoded parameter byte value: %u", b);
#endif
				*(unsigned char *)ptr = b;
				break;

			case CHAR:
				c = (char)bufferP[offset++];
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 6, true, "decoded parameter char value: %c", c);
#endif
				*(char *)ptr = c;
				break;

			case INT16:
				i16 = decode_int16(bufferP, offset);
				i16 = ntohs(i16);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 6, true, "decoded parameter int16 value: %d", i16);
#endif
				*(int16_t *)ptr = i16;
				break;

			case UINT16:
				ui16 = decode_uint16(bufferP, offset);
				ui16 = ntohs(ui16);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 6, true, "decoded parameter uint16 value: %u", ui16);
#endif
				*(uint16_t *)ptr = ui16;
				break;

			case INT32:
				i32 = decode_int32(bufferP, offset);
				i32 = ntohl(i32);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 6, true, "decoded parameter int32 value: %d", i32);
#endif
				*(int32_t *)ptr = i32;
				break;

			case UINT32:
				ui32 = decode_uint32(bufferP, offset);
				ui32 = ntohl(ui32);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 6, true, "decoded parameter uint32 value: %u", ui32);
#endif
				*(uint32_t *)ptr = ui32;
				break;

			case INT64:
				i64 = decode_int64(bufferP, offset);
				i64 = NTOHLL(i64);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 6, true, "decoded parameter int64 value: %l", i64);
#endif
				*(int64_t *)ptr = i64;
				break;

			case UINT64:
				ui64 = decode_uint64(bufferP, offset);
				ui64 = NTOHLL(ui64);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 6, true, "decoded parameter uint64 value: %lu", ui64);
#endif
				*(uint64_t *)ptr = ui64;
				break;

			case DOUBLE:
				ui64 = decode_uint64(bufferP, offset);
				ui64 = NTOHLL(ui64);
				d = *(double *)&ui64;
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 6, true, "decoded parameter double value: %f", d);
#endif
				*(double *)ptr = d;
				break;

			case STRING:
				ui16 = decode_uint16(bufferP, offset);
				ui16 = ntohs(ui16);
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 6, true, "decoded parameter string len: %u", ui16);
#endif
				s.clear();
				for (int i = 0; i < ui16; i ++)
					s += bufferP[offset++];
#ifdef RPC_TRACES
				LogVText(RPC_MODULE, 6, true, "decoded parameter string: %s", s.c_str());
#endif
				*(string *)ptr = s;
				break;

			case END_OF_CALL:
#ifdef RPC_TRACES
				LogText(RPC_MODULE, 6, true, "decoded end of stream marker");
#endif
				done = true;
				break;

			default:
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: unknown parameter type!" << endl;
				done = true;
				break;
		}
	}

	return true;
}

