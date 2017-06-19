/*
                     GNU LESSER GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.


  This version of the GNU Lesser General Public License incorporates
the terms and conditions of version 3 of the GNU General Public
License, supplemented by the additional permissions listed below.

  0. Additional Definitions.

  As used herein, "this License" refers to version 3 of the GNU Lesser
General Public License, and the "GNU GPL" refers to version 3 of the GNU
General Public License.

  "The Library" refers to a covered work governed by this License,
other than an Application or a Combined Work as defined below.

  An "Application" is any work that makes use of an interface provided
by the Library, but which is not otherwise based on the Library.
Defining a subclass of a class defined by the Library is deemed a mode
of using an interface provided by the Library.

  A "Combined Work" is a work produced by combining or linking an
Application with the Library.  The particular version of the Library
with which the Combined Work was made is also called the "Linked
Version".

  The "Minimal Corresponding Source" for a Combined Work means the
Corresponding Source for the Combined Work, excluding any source code
for portions of the Combined Work that, considered in isolation, are
based on the Application, and not on the Linked Version.

  The "Corresponding Application Code" for a Combined Work means the
object code and/or source code for the Application, including any data
and utility programs needed for reproducing the Combined Work from the
Application, but excluding the System Libraries of the Combined Work.

  1. Exception to Section 3 of the GNU GPL.

  You may convey a covered work under sections 3 and 4 of this License
without being bound by section 3 of the GNU GPL.

  2. Conveying Modified Versions.

  If you modify a copy of the Library, and, in your modifications, a
facility refers to a function or data to be supplied by an Application
that uses the facility (other than as an argument passed when the
facility is invoked), then you may convey a copy of the modified
version:

   a) under this License, provided that you make a good faith effort to
   ensure that, in the event an Application does not supply the
   function or data, the facility still operates, and performs
   whatever part of its purpose remains meaningful, or

   b) under the GNU GPL, with none of the additional permissions of
   this License applicable to that copy.

  3. Object Code Incorporating Material from Library Header Files.

  The object code form of an Application may incorporate material from
a header file that is part of the Library.  You may convey such object
code under terms of your choice, provided that, if the incorporated
material is not limited to numerical parameters, data structure
layouts and accessors, or small macros, inline functions and templates
(ten or fewer lines in length), you do both of the following:

   a) Give prominent notice with each copy of the object code that the
   Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the object code with a copy of the GNU GPL and this license
   document.

  4. Combined Works.

  You may convey a Combined Work under terms of your choice that,
taken together, effectively do not restrict modification of the
portions of the Library contained in the Combined Work and reverse
engineering for debugging such modifications, if you also do each of
the following:

   a) Give prominent notice with each copy of the Combined Work that
   the Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the Combined Work with a copy of the GNU GPL and this license
   document.

   c) For a Combined Work that displays copyright notices during
   execution, include the copyright notice for the Library among
   these notices, as well as a reference directing the user to the
   copies of the GNU GPL and this license document.

   d) Do one of the following:

       0) Convey the Minimal Corresponding Source under the terms of this
       License, and the Corresponding Application Code in a form
       suitable for, and under terms that permit, the user to
       recombine or relink the Application with a modified version of
       the Linked Version to produce a modified Combined Work, in the
       manner specified by section 6 of the GNU GPL for conveying
       Corresponding Source.

       1) Use a suitable shared library mechanism for linking with the
       Library.  A suitable mechanism is one that (a) uses at run time
       a copy of the Library already present on the user's computer
       system, and (b) will operate properly with a modified version
       of the Library that is interface-compatible with the Linked
       Version.

   e) Provide Installation Information, but only if you would otherwise
   be required to provide such information under section 6 of the
   GNU GPL, and only to the extent that such information is
   necessary to install and execute a modified version of the
   Combined Work produced by recombining or relinking the
   Application with a modified version of the Linked Version. (If
   you use option 4d0, the Installation Information must accompany
   the Minimal Corresponding Source and Corresponding Application
   Code. If you use option 4d1, you must provide the Installation
   Information in the manner specified by section 6 of the GNU GPL
   for conveying Corresponding Source.)

  5. Combined Libraries.

  You may place library facilities that are a work based on the
Library side by side in a single library together with other library
facilities that are not Applications and are not covered by this
License, and convey such a combined library under terms of your
choice, if you do both of the following:

   a) Accompany the combined library with a copy of the same work based
   on the Library, uncombined with any other library facilities,
   conveyed under the terms of this License.

   b) Give prominent notice with the combined library that part of it
   is a work based on the Library, and explaining where to find the
   accompanying uncombined form of the same work.

  6. Revised Versions of the GNU Lesser General Public License.

  The Free Software Foundation may publish revised and/or new versions
of the GNU Lesser General Public License from time to time. Such new
versions will be similar in spirit to the present version, but may
differ in detail to address new problems or concerns.

  Each version is given a distinguishing version number. If the
Library as you received it specifies that a certain numbered version
of the GNU Lesser General Public License "or any later version"
applies to it, you have the option of following the terms and
conditions either of that published version or of any later version
published by the Free Software Foundation. If the Library as you
received it does not specify a version number of the GNU Lesser
General Public License, you may choose any version of the GNU Lesser
General Public License ever published by the Free Software Foundation.

  If the Library as you received it specifies that a proxy can decide
whether future versions of the GNU Lesser General Public License shall
apply, that proxy's public statement of acceptance of any version is
permanent authorization for you to choose that version for the
Library.
*/

/**
 * \file RemoteProcedureCall.cpp
 *
 * \author  gilles fabre
 * \date Mar 14, 2017
 */

#include <stdio.h>
#include <stdarg.h>
#include <Link.h>
#include <iostream>

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
	v.push_back((val & 0xFF00000000000000) >> 56);
	v.push_back((val & 0x00FF000000000000) >> 48);
	v.push_back((val & 0x0000FF0000000000) >> 40);
	v.push_back((val & 0x000000FF00000000) >> 32);
	v.push_back((val & 0x00000000FF000000) >> 24);
	v.push_back((val & 0x0000000000FF0000) >> 16);
	v.push_back((val & 0x000000000000FF00) >> 8);
	v.push_back((val & 0x00000000000000FF));
}

/**
 * \fn void RemoteProcedureCall::push_uint64(vector<unsigned char> &v, uint64_t val)
 * \brief pushes a uint64 into the given serialization vector.
 *
 * \param v is a byte vector containing the serialized call stream
 * \param val is the data to serialize into the vector v
 */
inline void RemoteProcedureCall::push_uint64(vector<unsigned char> &v, uint64_t val) {
	v.push_back((val & 0xFF00000000000000) >> 56);
	v.push_back((val & 0x00FF000000000000) >> 48);
	v.push_back((val & 0x0000FF0000000000) >> 40);
	v.push_back((val & 0x000000FF00000000) >> 32);
	v.push_back((val & 0x00000000FF000000) >> 24);
	v.push_back((val & 0x0000000000FF0000) >> 16);
	v.push_back((val & 0x000000000000FF00) >> 8);
	v.push_back((val & 0x00000000000000FF));
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
 * \fn unsigned long RemoteProcedureCall::SerializeCall(string &func_name, int num_args, ...)
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
 * \param num_args is the number of parameters to pass to the function
 * \param ... is the corresponding number of parameters with the following
 *        format (<type><value>)|(PTR<pointer_to_value>)
 *        STRING values MUST be passed as string pointers (std::string*)
 * \return the unsigned long result passed to SerializedCallReturn
 */
inline unsigned long RemoteProcedureCall::SerializeCall(const string &func_name, int num_args, ...) {
	unsigned long result = 0;
	va_list 	  vl;

	va_start(vl, num_args);
	result = SerializeCall(func_name, num_args, vl);
	va_end(vl);

	return result;
}

unsigned long RemoteProcedureCall::SerializeCall(const string &func_name, int num_args, va_list vl) {
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
	cout << "\n RemoteProcedureCall::SerializeCall" << endl;
#endif

	// first pass the function name
	serialized_call.push_back(STRING);
	len = (unsigned int)func_name.length();
	push_uint16(serialized_call, htons(len));
	for (int i = 0; i < len; i++)
		serialized_call.push_back(func_name[i]);
#ifdef RPC_TRACES
	cout << "\t pushed func_name: " << func_name << endl;
#endif

	// then add the return result address
	serialized_call.push_back(UINT64);
	push_uint64(serialized_call, HTONLL((uint64_t)&result));
#ifdef RPC_TRACES
	cout << "\t pushed return result address: " << hex << &result << dec << endl;
#endif

	// and all the given parameters
	for (int i = 0; i < num_args;  i++) {
		char type = (char)va_arg(vl, int);

		// first push the parameter type in the call stream vector
		serialized_call.push_back(type);
#ifdef RPC_TRACES
		cout << "\t pushed parameter type: " << type << endl;
#endif

		// then, the corresponding data
		switch (type) {
			case PTR:
				// next parameter is a pointer
				--i;
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
					cout << "\t pushed parameter byte ptr: " << hex << ptr << dec << endl;
#endif
				} else {
					// a byte
					byte = (unsigned char)va_arg(vl, int);
					serialized_call.push_back(byte);
#ifdef RPC_TRACES
					cout << "\t pushed parameter byte: " << hex << "0x" << c << dec << endl;
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
					cout << "\t pushed parameter char ptr: " << hex << ptr << dec << endl;
#endif
				} else {
					// a char
					c = (char)va_arg(vl, int);
					serialized_call.push_back(c);
#ifdef RPC_TRACES
					cout << "\t pushed parameter char: " << hex << "0x" << c << dec << endl;
#endif
				}
				break;

			case INT16:
				if (is_ptr) {
					is_ptr = false;

					// an int16_t ptr
					ptr = (void *)va_arg(vl, int16_t *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					i16 = ptr ? *(int16_t *)ptr : 0;
					push_int16(serialized_call, ntohs(i16));
#ifdef RPC_TRACES
					cout << "\t pushed parameter int16_t ptr: " << hex << ptr << dec << endl;
#endif
				} else {
					// a short integer
					i16 = (int16_t)va_arg(vl, int);
					push_int16(serialized_call, htons(i16));
#ifdef RPC_TRACES
					cout << "\t pushed parameter int16_t: " << i16 << endl;
#endif
				}
				break;

			case UINT16:
				if (is_ptr) {
					is_ptr = false;

					// a uint16_t ptr
					ptr = (void *)va_arg(vl, uint16_t *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					ui16 = ptr ? *(uint16_t *)ptr : 0;
					push_uint16(serialized_call, ntohs(ui16));
#ifdef RPC_TRACES
					cout << "\t pushed parameter uint16_t ptr: " << hex << ptr << dec << endl;
#endif
				} else {
					// an unsigned short integer
					ui16 = (uint16_t)va_arg(vl, int);
					push_uint16(serialized_call, htons(ui16));
#ifdef RPC_TRACES
					cout << "\t pushed parameter uint16_t: " << ui16 << endl;
#endif
				}
				break;

			case INT32:
				if (is_ptr) {
					is_ptr = false;

					// an int32_t ptr
					ptr = (void *)va_arg(vl, int32_t *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					i32 = ptr ? *(int32_t *)ptr : 0;
					push_int32(serialized_call, ntohl(i32));
#ifdef RPC_TRACES
					cout << "\t pushed parameter int16_t ptr: " << hex << ptr << dec << endl;
#endif
				} else {
					// a long integer
					i32 = (int32_t)va_arg(vl, int32_t);
					push_int32(serialized_call, htonl(i32));
#ifdef RPC_TRACES
					cout << "\t pushed parameter int32_t: " << i32 << endl;
#endif
				}
				break;

			case UINT32:
				if (is_ptr) {
					is_ptr = false;

					// a uint32_t ptr
					ptr = (void *)va_arg(vl, uint32_t *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					ui32 = ptr ? *(uint32_t *)ptr : 0;
					push_uint32(serialized_call, ntohl(ui32));
#ifdef RPC_TRACES
					cout << "\t pushed parameter uint32_t ptr: " << hex << ptr << dec << endl;
#endif
				} else {
					// an unsigned long integer
					ui32 = (uint32_t)va_arg(vl, uint32_t);
					push_uint32(serialized_call, htonl(ui32));
#ifdef RPC_TRACES
					cout << "\t pushed parameter uint32_t: " << ui32 << endl;
#endif
				}
				break;

			case INT64:
				if (is_ptr) {
					is_ptr = false;

					// an int64_t ptr
					ptr = (void *)va_arg(vl, int64_t *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					i64 = ptr ? *(int64_t *)ptr : 0;
					push_int64(serialized_call, NTOHLL(i64));
#ifdef RPC_TRACES
					cout << "\t pushed parameter int64_t ptr: " << hex << ptr << dec << endl;
#endif
				} else {
					// a long long integer
					i64 = va_arg(vl, int64_t);
					push_int64(serialized_call, HTONLL(i64));
#ifdef RPC_TRACES
					cout << "\t pushed parameter int64_t: " << i64 << endl;
#endif
				}
				break;

			case UINT64:
				if (is_ptr) {
					is_ptr = false;

					// a uint64_t ptr
					ptr = (void *)va_arg(vl, uint64_t *);
					push_uint64(serialized_call, HTONLL((uint64_t)ptr));
					ui64 = ptr ? *(uint64_t *)ptr : 0;
					push_uint64(serialized_call, NTOHLL(ui64));
#ifdef RPC_TRACES
					cout << "\t pushed parameter uint64_t ptr: " << hex << ptr << dec << endl;
#endif
				} else {
					// an unsigned long long integer
					ui64 = va_arg(vl, uint64_t);
					push_uint64(serialized_call, HTONLL(ui64));
#ifdef RPC_TRACES
					cout << "\t pushed parameter uint64_t: " << ui64 << endl;
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
					cout << "\t pushed parameter string ptr: " << hex << ptr << dec << endl;
#endif
					// string
					s = ptr ? *(string *)ptr : "";
					len = (unsigned int)s.length();
					push_uint16(serialized_call, htons(len));
#ifdef RPC_TRACES
					cout << "\t pushed parameter string len: " << len << endl;
#endif
					for (int i = 0; i < len; i++)
						serialized_call.push_back(s[i]);
#ifdef RPC_TRACES
					cout << "\t pushed parameter string: " << s.substr(0, 10) << "..." << endl;
#endif
				} else {
					// string
					s = *va_arg(vl, string *);
					len = (unsigned int)s.length();
					push_uint16(serialized_call, htons(len));
#ifdef RPC_TRACES
					cout << "\t pushed parameter string len: " << len << endl;
#endif
					for (int i = 0; i < len; i++)
						serialized_call.push_back(s[i]);
#ifdef RPC_TRACES
					cout << "\t pushed parameter string: " << s.substr(0, 10) << "..." << endl;
#endif
				}
				break;
		}
	}

	// add end of call marker
	serialized_call.push_back(END_OF_CALL);
#ifdef RPC_TRACES
	cout << "\t pushed end of call stream marker" << endl << endl;
#endif

	// send all the serialized call parameters over to the peer
	unsigned long buff_len = serialized_call.size();
	SendPacket(serialized_call.data(), buff_len);

#ifdef RPC_TRACES
	cout << "\t sent " <<  buff_len << " bytes. Will now wait for reply... " << endl << endl;
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
	bufferP[offset++] = ((val & 0xFF00000000000000) >> 56);
	bufferP[offset++] = ((val & 0x00FF000000000000) >> 48);
	bufferP[offset++] = ((val & 0x0000FF0000000000) >> 40);
	bufferP[offset++] = ((val & 0x000000FF00000000) >> 32);
	bufferP[offset++] = ((val & 0x00000000FF000000) >> 24);
	bufferP[offset++] = ((val & 0x0000000000FF0000) >> 16);
	bufferP[offset++] = ((val & 0x000000000000FF00) >> 8);
	bufferP[offset++] = ((val & 0x00000000000000FF));
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
	uint64_t 		ui64;
	int16_t 		i16;
	int32_t 		i32;
	int64_t 		i64;
	int				offset = 0;
	bool 			done = false;
	string			s;
	bool			is_ptr = false;
#ifdef RPC_TRACES
	cout << "\n RemoteProcedureCall::DeserializeCall" << endl;
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
	cout << "\t decoded func_name len: " << ui16 << endl;
#endif

	// get the name
	for (int i = 0; i < ui16; i ++)
		func_name += bufferP[offset++];
#ifdef RPC_TRACES
	cout << "\t decoded func_name: " << func_name << endl;
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
	cout << "\t decoded return result address: " << hex << "0x" << ui64 << dec << endl;
#endif

	while (!done) {
		// get the parameter type
		type = bufferP[offset++];
#ifdef RPC_TRACES
		cout << "\t decoded arg type: " << type << endl;
#endif
		switch ((ParamType)type) {
			case PTR:
				// next parameter is a pointer
				is_ptr = true;
				break;

			case BYTE:
				if (is_ptr) {
					is_ptr = false;
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
#ifdef RPC_TRACES
					cout << "\t decoded ptr: " << hex << "0x" << ui64 << dec << endl;
#endif
					b = bufferP[offset++];
					resultP->push_back(new Parameter(b, ui64));
#ifdef RPC_TRACES
					cout << "\t decoded byte: " << hex << "0x" << b << dec << endl;
#endif
				} else {
					b = bufferP[offset++];
					resultP->push_back(new Parameter(b));
#ifdef RPC_TRACES
					cout << "\t decoded byte: " << hex << "0x" << b << dec << endl;
#endif
				}
				break;

			case CHAR:
				if (is_ptr) {
					is_ptr = false;
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
#ifdef RPC_TRACES
					cout << "\t decoded ptr: " << hex << "0x" << ui64 << dec << endl;
#endif
					c = bufferP[offset++];
					resultP->push_back(new Parameter(c, ui64));
#ifdef RPC_TRACES
					cout << "\t decoded byte: " << hex << "0x" << b << dec << endl;
#endif
				} else {
					c = bufferP[offset++];
					resultP->push_back(new Parameter(c));
#ifdef RPC_TRACES
					cout << "\t decoded char: " << hex << "0x" << c << dec << endl;
#endif
				}
				break;

			case INT16:
				if (is_ptr) {
					is_ptr = false;
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
#ifdef RPC_TRACES
					cout << "\t decoded ptr: " << hex << "0x" << ui64 << dec << endl;
#endif
					i16 = decode_int16(bufferP, offset);
					i16 = ntohs(i16);
					resultP->push_back(new Parameter(i16, ui64));
#ifdef RPC_TRACES
					cout << "\t decoded int16_t: " << i16 << endl;
#endif
				} else {
					i16 = decode_int16(bufferP, offset);
					i16 = ntohs(i16);
					resultP->push_back(new Parameter(i16));
#ifdef RPC_TRACES
					cout << "\t decoded int16_t: " << i16 << endl;
#endif
				}
				break;

			case UINT16:
				if (is_ptr) {
					is_ptr = false;
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
#ifdef RPC_TRACES
					cout << "\t decoded ptr: " << hex << "0x" << ui64 << dec << endl;
#endif
					ui16 = decode_uint16(bufferP, offset);
					ui16 = ntohs(ui16);
					resultP->push_back(new Parameter(ui16, ui64));
#ifdef RPC_TRACES
					cout << "\t decoded int16_t: " << i16 << endl;
#endif
				} else {
					ui16 = decode_uint16(bufferP, offset);
					ui16 = ntohs(ui16);
					resultP->push_back(new Parameter(ui16));
#ifdef RPC_TRACES
					cout << "\t decoded uint16_t: " << ui16 << endl;
#endif
				}
				break;

			case INT32:
				if (is_ptr) {
					is_ptr = false;
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
#ifdef RPC_TRACES
					cout << "\t decoded ptr: " << hex << "0x" << ui64 << dec << endl;
#endif
					i32 = decode_int32(bufferP, offset);
					i32 = ntohl(i32);
					resultP->push_back(new Parameter(i32, ui64));
#ifdef RPC_TRACES
					cout << "\t decoded int32_t: " << i32 << endl;
#endif
				} else {
					i32 = decode_int32(bufferP, offset);
					i32 = ntohl(i32);
					resultP->push_back(new Parameter(i32));
#ifdef RPC_TRACES
					cout << "\t decoded int32_t: " << i32 << endl;
#endif
				}
				break;

			case UINT32:
				if (is_ptr) {
					is_ptr = false;
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
#ifdef RPC_TRACES
					cout << "\t decoded ptr: " << hex << "0x" << ui64 << dec << endl;
#endif
					ui32 = decode_uint32(bufferP, offset);
					ui32 = ntohl(ui32);
					resultP->push_back(new Parameter(ui32, ui64));
#ifdef RPC_TRACES
					cout << "\t decoded uint32_t: " << ui32 << endl;
#endif
				} else {
					ui32 = decode_uint32(bufferP, offset);
					ui32 = ntohl(ui32);
					resultP->push_back(new Parameter(ui32));
#ifdef RPC_TRACES
					cout << "\t decoded uint32_t: " << ui32 << endl;
#endif
				}
				break;

			case INT64:
				if (is_ptr) {
					is_ptr = false;
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
#ifdef RPC_TRACES
					cout << "\t decoded ptr: " << hex << "0x" << ui64 << dec << endl;
#endif
					i64 = decode_int64(bufferP, offset);
					i64 = NTOHLL(i64);
					resultP->push_back(new Parameter(i64, ui64));
#ifdef RPC_TRACES
					cout << "\t decoded int64_t: " << i64 << endl;
#endif
				} else {
					i64 = decode_int64(bufferP, offset);
					i64 = NTOHLL(i64);
					resultP->push_back(new Parameter(i64));
#ifdef RPC_TRACES
					cout << "\t decoded int64_t: " << i64 << endl;
#endif
				}
				break;

			case UINT64:
				if (is_ptr) {
					is_ptr = false;
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
#ifdef RPC_TRACES
					cout << "\t decoded ptr: " << hex << "0x" << ui64 << dec << endl;
#endif
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
					resultP->push_back(new Parameter(ui64, ui64));
#ifdef RPC_TRACES
					cout << "\t decoded uint64_t: " << ui64 << endl;
#endif
				} else {
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
					resultP->push_back(new Parameter(ui64));
#ifdef RPC_TRACES
					cout << "\t decoded uint64_t: " << ui64 << endl;
#endif
				}
				break;


			case STRING:
				if (is_ptr) {
					is_ptr = false;
					ui64 = decode_uint64(bufferP, offset);
					ui64 = NTOHLL(ui64);
#ifdef RPC_TRACES
					cout << "\t decoded ptr: " << hex << "0x" << ui64 << dec << endl;
#endif
					ui16 = decode_uint16(bufferP, offset);
					ui16 = ntohs(ui16);
#ifdef RPC_TRACES
					cout << "\t decoded string len: " << ui16 << endl;
#endif
					s.clear();
					for (int i = 0; i < ui16; i ++)
						s += bufferP[offset++];
					resultP->push_back(new Parameter(s.c_str(), ui64));
#ifdef RPC_TRACES
					cout << "\t decoded string: " << s.substr(0, 10) << "..." << endl;
#endif
				} else {
					ui16 = decode_uint16(bufferP, offset);
					ui16 = ntohs(ui16);
#ifdef RPC_TRACES
					cout << "\t decoded string len: " << ui16 << endl;
#endif
					s.clear();
					for (int i = 0; i < ui16; i ++)
						s += bufferP[offset++];
					resultP->push_back(new Parameter(s.c_str()));
#ifdef RPC_TRACES
					cout << "\t decoded string: " << s.substr(0, 10) << "..." << endl;
#endif
				}
				break;

			case END_OF_CALL:
				// marks the end of the parameters (and call stream)
				done = true;
#ifdef RPC_TRACES
				cout << "\t decoded end of call stream marker" << endl << endl;
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
	string					s;
#ifdef RPC_TRACES
	cout << "\n RemoteProcedureCall::SerializeCallReturn" << endl;
#endif

	serialized_call.push_back(UINT64);

	ui64 = paramP->at(0)->GetUInt64Value();
	push_uint64(serialized_call, HTONLL(ui64));
#ifdef RPC_TRACES
	cout << "\t pushed return result address: " << hex << "0x" << ui64 << dec << endl;
#endif

	ui64 = (uint64_t)ret_val;
	serialized_call.push_back(UINT64);
	push_uint64(serialized_call, HTONLL(ui64));
#ifdef RPC_TRACES
	cout << "\t pushed return result: " << ui64 << endl;
#endif

	// push all ptr parameters back
	for (vector<Parameter *>::iterator i = paramP->begin(); i != paramP->end(); i++) {
		if (!(*i)->IsValidPointer())
			continue;

		// push ptr type
		serialized_call.push_back(PTR);
#ifdef RPC_TRACES
		cout << "\t\t pushed ptr type" << endl;
#endif

		// push ptr
		ui64 = (*i)->GetCallerPointer();
		push_uint64(serialized_call, HTONLL(ui64));
#ifdef RPC_TRACES
		cout << "\t\t pushed parameter ptr: " << hex << "0x" << ui64 << dec << endl;
#endif

		// push type
		type = (unsigned char)(*i)->GetType();
		serialized_call.push_back(type);
#ifdef RPC_TRACES
		cout << "\t\t pushed parameter type: " << type << endl;
#endif

		// push value
		switch (type) {
			case BYTE:
				b = (*i)->GetByteValue();
				serialized_call.push_back(b);
#ifdef RPC_TRACES
				cout << "\t\t pushed byte parameter value: " << hex << "0x" << b << dec << endl;
#endif
				break;

			case CHAR:
				c = (*i)->GetCharValue();
				serialized_call.push_back(c);
#ifdef RPC_TRACES
				cout << "\t\t pushed char parameter value: " << hex << "0x" << c << dec << endl;
#endif
				break;

			case INT16:
				i16 = (*i)->GetInt16Value();
				push_int16(serialized_call, htons(i16));
#ifdef RPC_TRACES
				cout << "\t\t pushed int16 parameter value: " << i16 << endl;
#endif
				break;

			case UINT16:
				ui16 = (*i)->GetUInt16Value();
				push_uint16(serialized_call, htons(ui16));
#ifdef RPC_TRACES
				cout << "\t\t pushed uint16 parameter value: " << ui16 << endl;
#endif
				break;

			case INT32:
				i32 = (*i)->GetInt32Value();
				push_int32(serialized_call, htonl(i32));
#ifdef RPC_TRACES
				cout << "\t\t pushed int32 parameter value: " << i32 << endl;
#endif
				break;

			case UINT32:
				ui32 = (*i)->GetUInt32Value();
				push_uint32(serialized_call, htonl(ui32));
#ifdef RPC_TRACES
				cout << "\t\t pushed uint32 parameter value: " << ui32 << endl;
#endif
				break;

			case INT64:
				i64 = (*i)->GetInt64Value();
				push_int64(serialized_call, HTONLL(i64));
#ifdef RPC_TRACES
				cout << "\t\t pushed int64 parameter value: " << i64 << endl;
#endif
				break;

			case UINT64:
				ui64 = (*i)->GetUInt64Value();
				push_uint64(serialized_call, HTONLL(ui64));
#ifdef RPC_TRACES
				cout << "\t\t pushed uint64 parameter value: " << ui64 << endl;
#endif
				break;

			case STRING:
				s = (*i)->GetStringValue();
				ui16 = s.length();
				push_uint16(serialized_call, htons(ui16));
#ifdef RPC_TRACES
				cout << "\t\t pushed string parameter value len: " << ui16 << endl;
#endif

				for (int i = 0; i < ui16; i++)
					serialized_call.push_back(s[i]);
#ifdef RPC_TRACES
				cout << "\t\t pushed string parameter value: " << s.substr(0, 10) << endl;
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
	cout << "\t pushed end of call stream marker" << endl << endl;
#endif

	// send all the serialized call parameters over to the peer
	SendPacket(serialized_call.data(), serialized_call.size());
#ifdef RPC_TRACES
	cout << "\t sent " <<  serialized_call.size() << " bytes... " << endl << endl;
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
	string			s;
	bool			done = false;
#ifdef RPC_TRACES
	cout << "\n RemoteProcedureCall::DeserializeCallReturn" << endl;
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
	cout << "\t decoded return result address: " << hex << "0x" << ui64 << dec << endl;
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
	cout << "\t decoded result: " << ui64 << endl;
#endif

	while (!done) {
		type = bufferP[offset++];
#ifdef RPC_TRACES
		cout << "\t\t decoded parameter type: " << type << endl;
#endif
		switch (type) {
			case PTR:
				// get ptr
				ui64 = decode_uint64(bufferP, offset);
				ptr = NTOHLL(ui64);
#ifdef RPC_TRACES
				cout << "\t\t decoded parameter ptr: " << hex << "0x" << ptr << dec << endl;
#endif
				break;

			case BYTE:
				b = (unsigned char)bufferP[offset++];
#ifdef RPC_TRACES
				cout << "\t\t decoded parameter byte value: " << hex << b << dec << endl;
#endif
				*(unsigned char *)ptr = b;
				break;

			case CHAR:
				c = (char)bufferP[offset++];
#ifdef RPC_TRACES
				cout << "\t\t decoded parameter char value: " << hex << c << dec << endl;
#endif
				*(char *)ptr = c;
				break;

			case INT16:
				i16 = decode_int16(bufferP, offset);
				i16 = ntohs(i16);
#ifdef RPC_TRACES
				cout << "\t\t decoded parameter int16 value: " << i16 << endl;
#endif
				*(int16_t *)ptr = i16;
				break;

			case UINT16:
				ui16 = decode_uint16(bufferP, offset);
				ui16 = ntohs(ui16);
#ifdef RPC_TRACES
				cout << "\t\t decoded parameter uint16 value: " << ui16 << endl;
#endif
				*(uint16_t *)ptr = ui16;
				break;

			case INT32:
				i32 = decode_int32(bufferP, offset);
				i32 = ntohl(i32);
#ifdef RPC_TRACES
				cout << "\t\t decoded parameter int32 value: " << i32 << endl;
#endif
				*(int32_t *)ptr = i32;
				break;

			case UINT32:
				ui32 = decode_uint32(bufferP, offset);
				ui32 = ntohl(ui32);
#ifdef RPC_TRACES
				cout << "\t\t decoded parameter uint32 value: " << ui32 << endl;
#endif
				*(uint32_t *)ptr = ui32;
				break;

			case INT64:
				i64 = decode_int64(bufferP, offset);
				i64 = NTOHLL(i64);
#ifdef RPC_TRACES
				cout << "\t\t decoded parameter int64 value: " << i64 << endl;
#endif
				*(int64_t *)ptr = i64;
				break;

			case UINT64:
				ui64 = decode_uint64(bufferP, offset);
				ui64 = NTOHLL(ui64);
#ifdef RPC_TRACES
				cout << "\t\t decoded parameter uint64 value: " << ui64 << endl;
#endif
				*(uint64_t *)ptr = ui64;
				break;

			case STRING:
				ui16 = decode_uint16(bufferP, offset);
				ui16 = ntohs(ui16);
#ifdef RPC_TRACES
				cout << "\t\t decoded parameter string len: " << ui16 << endl;
#endif
				s.clear();
				for (int i = 0; i < ui16; i ++)
					s += bufferP[offset++];
#ifdef RPC_TRACES
				cout << "\t\t decoded parameter string: " << s.substr(0, 10) << "..." << endl;
#endif
				*(string *)ptr = s;
				break;

			case END_OF_CALL:
#ifdef RPC_TRACES
				cout << "\t\t decoded end of call stream marker" << endl;
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

