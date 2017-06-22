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
 * \file RemoteProcedureCall.h
 *
 * \author  gilles fabre
 * \date Mar 14, 2017
 */

#ifndef _RPC_REMOTEPROCEDURECALL_H_
#define _RPC_REMOTEPROCEDURECALL_H_

#include <stdarg.h>
#include <Link.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include <string>
#include <vector>

using namespace std;

#define RPC_TRACES 1

#define HTONLL(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define NTOHLL(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))

/**
 * \class RemoteProcedureCall
 * \brief Instantiate an object of this class with a Link to
 *        pass calls to the connected peer and retrieve a
 *        result. Calls are blocking, parameters are 'in' or 'out'.
 *        out parameters must be passed as pointers, prefixed by
 *        a	PTR type.
 *
 */
class RemoteProcedureCall {
	Link *m_linkP; // the pointer to the link conveying the calls

	// invoked internally (by SerializeCall) upon rpc reply receipt
	bool 		DeserializeCallReturn(unsigned char *bufferP);

	// exchange sized call streams
	bool			SendPacket(unsigned char *bufferP, unsigned long data_len);
	unsigned char 	*ReceivePacket(unsigned long &data_len);

	// utility methods to push parameter values [and pointers] into the
	// call stream serialization vector and read/decode parameter
	// values [and pointers] from the serialization call return byte stream.
	void 		push_int64(vector<unsigned char> &s, int64_t val);
	void 		push_uint64(vector<unsigned char> &s, uint64_t val);
	int64_t 	decode_int64(unsigned char *bufferP, int &offset);
	uint64_t 	decode_uint64(unsigned char *bufferP, int &offset);
	void		encode_uint64(uint64_t val, unsigned char *bufferP, int &offset);

	void 		push_int32(vector<unsigned char> &s, int32_t val);
	void 		push_uint32(vector<unsigned char> &s, uint32_t val);
	int32_t 	decode_int32(unsigned char *bufferP, int &offset);
	uint32_t 	decode_uint32(unsigned char *bufferP, int &offset);

	void 		push_int16(vector<unsigned char> &s, int16_t val);
	void 		push_uint16(vector<unsigned char> &s, uint16_t val);
	int16_t 	decode_int16(unsigned char *bufferP, int &offset);
	uint16_t 	decode_uint16(unsigned char *bufferP, int &offset);

public:
	// the parameter types
	enum ParamType {PTR 		= 'P', // if present before type, this is a pointer parameter
					BYTE   		= 'b',
					CHAR   		= 'c',
					INT16  		= 'i',
					UINT16 		= 'I',
					INT32  		= 'l',
					UINT32 		= 'L',
					INT64  		= 'h',
					UINT64 		= 'H',
					DOUBLE		= 'D',
					STRING   	= 's',
					RESULT_ADDRESS = 'R',
					END_OF_CALL = 'X'};

	/**
	 * \class Parameter
	 * \brief Instances of this class are passed to the caller of
	 * 		  the DeserializeCall method, describing and providing
	 * 		  access to the parameters passed through the call stream
	 * 		  built by SerializeCall. Out parameters can be modified
	 * 		  and their modified value will transparently be passed
	 * 		  back to the SerializeCall caller.
	 */
	class Parameter {
	private:
		ParamType m_type;
		uint64_t	  		m_caller_valP; // caller's side ptr, to transmit back upon return;
		union {
			char 		   _char;
			unsigned char  _byte;
			int16_t		   _i16;
			uint16_t	   _ui16;
			int32_t		   _i32;
			uint32_t	   _ui32;
			int64_t		   _i64;
			uint64_t	   _ui64;
			double		   _double;
			string		   *_stringP;
		} m_value;

	public:
		// parameter constructors, invoked depending on
		// the serialized parameter type and pointer
		// presence (for an output parameter).
		Parameter(unsigned char value, uint64_t caller_valP = 0) {
			m_caller_valP = caller_valP;
			memset(&m_value, 0, sizeof(m_value));
			m_type = BYTE;
			m_value._byte = value;
		}

		Parameter(char value, uint64_t caller_valP = 0) {
			m_caller_valP = caller_valP;
			memset(&m_value, 0, sizeof(m_value));
			m_type = CHAR;
			m_value._char = value;
		}

		Parameter(int16_t value, uint64_t caller_valP = 0) {
			m_caller_valP = caller_valP;
			memset(&m_value, 0, sizeof(m_value));
			m_type = INT16;
			m_value._i16 = value;
		}

		Parameter(uint16_t value, uint64_t caller_valP = 0) {
			m_caller_valP = caller_valP;
			memset(&m_value, 0, sizeof(m_value));
			m_type = UINT16;
			m_value._ui16 = value;
		}

		Parameter(int32_t value, uint64_t caller_valP = 0) {
			m_caller_valP = caller_valP;
			memset(&m_value, 0, sizeof(m_value));
			m_type = INT32;
			m_value._i32 = value;
		}

		Parameter(uint32_t value, uint64_t caller_valP = 0) {
			m_caller_valP = caller_valP;
			memset(&m_value, 0, sizeof(m_value));
			m_type = UINT32;
			m_value._ui32 = value;
		}

		Parameter(int64_t value, uint64_t caller_valP = 0) {
			m_caller_valP = caller_valP;
			memset(&m_value, 0, sizeof(m_value));
			m_type = INT64;
			m_value._i64 = value;
		}

		Parameter(uint64_t value, uint64_t caller_valP = 0) {
			m_caller_valP = caller_valP;
			memset(&m_value, 0, sizeof(m_value));
			m_type = UINT64;
			m_value._ui64 = value;
		}

		Parameter(double value, uint64_t caller_valP = 0) {
			m_caller_valP = caller_valP;
			memset(&m_value, 0, sizeof(m_value));
			m_type = DOUBLE;
			m_value._double = value;
		}

		Parameter(const char *value, uint64_t caller_valP = 0) {
			m_caller_valP = caller_valP;
			memset(&m_value, 0, sizeof(m_value));
			m_type = STRING;
			m_value._stringP = new string();
			*m_value._stringP = value;
		}

		// if destroying a STRING parameter, the string object
		// must be freed.
		~Parameter() {
			if (m_type == STRING && m_value._stringP)
				delete m_value._stringP;
		}

		ParamType GetType() {return m_type;}

		// parameter getters, return a reference to the embedded
		// parameter value such that it can be modified
		char &GetCharValue() {
			return m_value._char;
		}

		unsigned char &GetByteValue() {
			return m_value._byte;
		}

		int16_t &GetInt16Value() {
			return m_value._i16;
		}

		uint16_t &GetUInt16Value() {
			return m_value._ui16;
		}

		int32_t &GetInt32Value() {
			return m_value._i32;
		}

		uint32_t &GetUInt32Value() {
			return m_value._ui32;
		}

		int64_t &GetInt64Value() {
			return m_value._i64;
		}

		uint64_t &GetUInt64Value() {
			return m_value._ui64;
		}

		double &GetDoubleValue() {
			return m_value._double;
		}

		string &GetStringValue() {
			return *m_value._stringP;
		}

		// pointer getters
		bool IsValidPointer() {
			return m_caller_valP != 0;
		}

		uint64_t GetCallerPointer() {
			return m_caller_valP;
		}
	};

	// RPC constructor keeps the link reference
	// the link is owned by the transport which will
	// close and delete it upon destruction
	RemoteProcedureCall(Link *linkP) {
		m_linkP = linkP;
	}

	// rpc call caller side

	// call an rpc function
	unsigned long 		 SerializeCall(const string &func_name, ...);
	unsigned long 		 SerializeCall(const string &func_name, va_list vl);

	// rpc function callee side

	// analyze an rpc function call
	vector<Parameter *>* DeserializeCall(string &func_name);

	// send back rpc function result
	void 				 SerializeCallReturn(vector<Parameter *>* paramP, unsigned long ret_val);

	bool IsConnected() {
		return m_linkP && m_linkP->IsConnected();
	}

	void Close() {
		if (m_linkP)
			m_linkP->Close();
	}
};


#endif // _RPC_REMOTEPROCEDURECALL_H_
