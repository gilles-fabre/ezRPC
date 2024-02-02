#ifndef _RPC_REMOTEPROCEDURECALL_H_
#define _RPC_REMOTEPROCEDURECALL_H_

#include <stdarg.h>
#include <Link.h>
#include <string.h>
#include <stdlib.h>

#ifndef WIN32
#include <arpa/inet.h>
#endif

#include <string>
#include <vector>

using namespace std;

#define RPC_TRACES 1
#define RPC_MODULE 0x4

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
	Link* m_linkP; // the pointer to the link conveying the calls

	// invoked internally (by SerializeCall) upon rpc reply receipt
	bool DeserializeCallReturn(unsigned char* bufferP);

	// exchange sized call streams
	bool						SendPacket(unsigned char* bufferP, unsigned long data_len);
	unsigned char*  ReceivePacket(unsigned long& data_len);

	// utility methods to push parameter values [and pointers] into the
	// call stream serialization vector and read/decode parameter
	// values [and pointers] from the serialization call return byte stream.
	void 			push_int64(vector<unsigned char>& s, int64_t val);
	void 			push_uint64(vector<unsigned char>& s, uint64_t val);
	int64_t 	decode_int64(unsigned char* bufferP, int& offset);
	uint64_t 	decode_uint64(unsigned char* bufferP, int& offset);
	void			encode_uint64(uint64_t val, unsigned char* bufferP, int& offset);

	void 			push_int32(vector<unsigned char>& s, int32_t val);
	void 			push_uint32(vector<unsigned char>& s, uint32_t val);
	int32_t 	decode_int32(unsigned char* bufferP, int& offset);
	uint32_t 	decode_uint32(unsigned char* bufferP, int& offset);

	void 			push_int16(vector<unsigned char>& s, int16_t val);
	void 			push_uint16(vector<unsigned char>& s, uint16_t val);
	int16_t 	decode_int16(unsigned char* bufferP, int& offset);
	uint16_t 	decode_uint16(unsigned char* bufferP, int& offset);

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
		ParamType				m_type;
		uint64_t	  		m_caller_valP; // caller's side ptr, to transmit back upon return;
		union {
			char 					_char;
			unsigned char _byte;
			int16_t		    _i16;
			uint16_t	    _ui16;
			int32_t		    _i32;
			uint32_t	    _ui32;
			int64_t		    _i64;
			uint64_t	    _ui64;
			double		    _double;
			string*				_stringP;
		} m_value;

	public:
		/**
		 * \fn Parameter(<type> value, uint64_t caller_valP = 0)
		 * \brief parameter constructors, invoked depending on 
		 * 		  the serialized parameter type and pointer
		 * 		  presence (for an output parameter).
		 * 
		 * \param value is the parameter value
		 * \param caller_valP is the shadow parameter address (to return a 
		 * 					  valuet the caller if set). Set to 0 by default.
		*/
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

		/**
		 * \fn ~Parameter
		 * \brief parameter destructor. if destroying a STRING parameter, 
		 * 		  the string objectmust be freed.
		 * 
		*/
		~Parameter() {
			if (m_type == STRING && m_value._stringP)
				delete m_value._stringP;
		}

		ParamType GetType() {return m_type;}

		/**
		 * \fn Get<Type>Reference
		 * \brief parameter getters, return a reference to the embedded
		 * 		  parameter value such that it can be modified.
		*/
		char &GetCharReference() {
			return m_value._char;
		}

		unsigned char &GetByteReference() {
			return m_value._byte;
		}

		int16_t &GetInt16Reference() {
			return m_value._i16;
		}

		uint16_t &GetUInt16Reference() {
			return m_value._ui16;
		}

		int32_t &GetInt32Reference() {
			return m_value._i32;
		}

		uint32_t &GetUInt32Reference() {
			return m_value._ui32;
		}

		int64_t &GetInt64Reference() {
			return m_value._i64;
		}

		uint64_t &GetUInt64Reference() {
			return m_value._ui64;
		}

		double &GetDoubleReference() {
			return m_value._double;
		}

		string &GetStringReference() {
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

	/**
	 * \fn RemoteProcedureCall(Link *linkP)
	 * \brief RPC constructor keeps the link reference
	 * 		  the link is owned by the transport which will
	 * 		  close and delete it upon destruction.
	 * 
	 * \param linkP is the transport link to the server.
	*/
	RemoteProcedureCall(Link* linkP) {
		m_linkP = linkP;
	}

	// rpc call caller side

	/**
	 * \fn unsigned long SerializeCall(string func_name, ...)
	 * \brief Serializes a call to the server.
	 * 
	 * \param func_name is the name of the Remote Procedure to call
	 * 					on the server side.
	 * \param ... variadic, ended by RemoteProcedure::END_OF_CALL, see
	 * 		      ParamType for possible values.
	*/
	unsigned long 		 SerializeCall(string func_name, ...);
	unsigned long 		 SerializeCall(const string& func_name, va_list vl);

	// rpc function callee side

	/**
	 * \fn vector<Parameter*>* DeserializeCall(string& func_name)
	 * \brief Deserializes a call into the server.
	 * 
	 * \param func_name is the name of the Remote Procedure to call
	 * 					on the server side.
	*/
	vector<Parameter*>* DeserializeCall(string& func_name);

	/**
	 * \fn
	 * \brief Send back the Remote Procedure result
	 * 
	 * \param paramP the vector<Parameter *>* containing the passed parameters.
	*/
	void SerializeCallReturn(vector<Parameter*>* paramP, unsigned long ret_val);

	void Close() {
		if (m_linkP)
			m_linkP->Close();
	}
};

typedef unsigned long RemoteProcedure(vector<RemoteProcedureCall::Parameter*>*, void* user_dataP);

#endif // _RPC_REMOTEPROCEDURECALL_H_
