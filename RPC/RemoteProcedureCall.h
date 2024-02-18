#ifndef _RPC_REMOTEPROCEDURECALL_H_
#define _RPC_REMOTEPROCEDURECALL_H_

#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>

#include <stdarg.h>
#include <Link.h>
#include <string.h>
#include <stdlib.h>
#include <Thread.h>

#ifndef WIN32
#include <arpa/inet.h>
#endif

using namespace std;

#ifdef _DEBUG
#define RPC_TRACES 1
#endif

#define RPC_MODULE 0x4

#define HTONLL(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define NTOHLL(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))

typedef uint64_t AsyncID;
#define INVALID_ASYNC_ID (AsyncID)0

/**
 * \class RemoteProcedureCall
 * \brief Instantiate an object of this class with a Link to pass calls to the connected peer and retrieve a
 *        result. Calls are either synchronous or asynchronous, parameters are 'in' or 'out'.
 *        out parameters must be passed as pointers, prefixed by a PTR type.
 *
 */
class RemoteProcedureCall {
	Link* m_linkP;			 // the pointer to the link conveying the calls
	mutex m_cli_send_mutex;		 // protect the transport against concurrent send (async calls multiplexing)
	mutex m_cli_receive_mutex;	 // protect the transport against concurrent receive (async calls multiplexing)
	mutex m_srv_send_mutex;		 // protect the transport against concurrent send (async calls multiplexing)
	mutex m_srv_receive_mutex;	 // protect the transport against concurrent receive (async calls multiplexing)

	// invoked internally (by SerializeCall) upon rpc reply receipt
	bool DeserializeCallReturn(AsyncID& asyncId, unsigned char* bufferP);

	// exchange sized call streams
	bool			SendPacket(unsigned char* bufferP, unsigned long data_len);
	unsigned char*  ReceivePacket(unsigned long& data_len);

	// utility methods to push parameter values [and pointers] into the
	// call stream serialization vector and read/decode parameter
	// values [and pointers] from the serialization call return byte stream.
	void 		push_int64(vector<unsigned char>& s, int64_t val);
	void 		push_uint64(vector<unsigned char>& s, uint64_t val);
	int64_t 	decode_int64(unsigned char* bufferP, int& offset);
	uint64_t 	decode_uint64(unsigned char* bufferP, int& offset);
	void		encode_uint64(uint64_t val, unsigned char* bufferP, int& offset);

	void 		push_int32(vector<unsigned char>& s, int32_t val);
	void 		push_uint32(vector<unsigned char>& s, uint32_t val);
	int32_t 	decode_int32(unsigned char* bufferP, int& offset);
	uint32_t 	decode_uint32(unsigned char* bufferP, int& offset);

	void 		push_int16(vector<unsigned char>& s, int16_t val);
	void 		push_uint16(vector<unsigned char>& s, uint16_t val);
	int16_t 	decode_int16(unsigned char* bufferP, int& offset);
	uint16_t 	decode_uint16(unsigned char* bufferP, int& offset);

public:
	// the parameter types
	enum ParamType {PTR 		= 'P', // if present before type, this is a pointer parameter
					BYTE   		= 'b',
					BYTE_REF	= 'B',
					CHAR   		= 'c',
					CHAR_REF	= 'C',
					INT16  		= 'i',
					INT16_REF   = 'I',
					UINT16 		= 'j',
					UINT16_REF  = 'J',
					INT32  		= 'k',
					INT32_REF	= 'K',
					UINT32 		= 'l',
					UINT32_REF	= 'L',
					INT64  		= 'm',
					INT64_REF	= 'M',
					UINT64 		= 'n',
					UINT64_REF	= 'N',
					DOUBLE		= 'o',
					DOUBLE_REF	= 'O',
					STRING   	= 'p',
					STRING_REF	= 'P',
					RESULT_ADDRESS = 'R',
					END_OF_CALL = 'X',
					ASYNC_ID    = 'A',
					EMPTY};

	static const unordered_map<string, ParamType> m_types;

	/**
	 * \class Parameter
	 * \brief Instances of this class are passed to the caller of
	 * 		  the DeserializeCall method, describing and providing
	 * 		  access to the parameters passed through the call stream
	 * 		  built by SerializeCall. Out parameters can be modified
	 * 		  and their modified value will transparently be passed
	 * 		  back to the SerializeCall caller.
	 */
	class ParameterBase {
	protected:
		ParamType 		m_type;
		uint64_t		m_caller_valP;

	public:
		ParameterBase(ParamType type, uint64_t caller_valP) {
			m_caller_valP = caller_valP;
			m_type = type;
		}

	    virtual ~ParameterBase() = default; // just so we can dynamic_cast ParameterBase* into Parameter<T>*

		// type accessor
		ParamType GetType() {return m_type;}

		// pointer getters
		bool IsValidPointer() {
			return m_caller_valP != 0;
		}

		uint64_t GetCallerPointer() {
			return m_caller_valP;
		}
	};

	template <typename T>
	class Parameter : public ParameterBase {
	private:
		T m_value;

	public:
		/**
		 * \fn Parameter(<type> value, void* caller_valP = 0)
		 * \brief parameter constructors, invoked depending on 
		 * 		  the serialized parameter type and pointer
		 * 		  presence (for an output parameter).
		 * 
		 * \param value is the parameter value
		 * \param caller_valP is the shadow parameter address (to return a 
		 * 					  value to the caller if set). Set to 0 by default.
		*/
		Parameter(ParamType type) : ParameterBase(type, 0) {
			m_value = type;
		}

		Parameter(unsigned char value, uint64_t caller_valP = 0) : ParameterBase(BYTE, caller_valP) {
			m_value = value;
		}

		Parameter(char value, uint64_t caller_valP = 0) : ParameterBase(CHAR, caller_valP){
			m_value = value;
		}

		Parameter(int16_t value, uint64_t caller_valP = 0) : ParameterBase(INT16, caller_valP) {
			m_value = value;
		}

		Parameter(uint16_t value, uint64_t caller_valP = 0) : ParameterBase(UINT16, caller_valP) {
			m_value = value;
		}

		Parameter(int32_t value, uint64_t caller_valP = 0) : ParameterBase(INT32, caller_valP) {
			m_value = value;
		}

		Parameter(uint32_t value, uint64_t caller_valP = 0) : ParameterBase(UINT32, caller_valP) {
			m_value = value;
		}

		Parameter(int64_t value, uint64_t caller_valP = 0) : ParameterBase(INT64, caller_valP) {
			m_value = value;
		}

		Parameter(uint64_t value, uint64_t caller_valP = 0) : ParameterBase(UINT64, caller_valP) {
			m_value = value;
		}

		Parameter(double value, uint64_t caller_valP = 0) : ParameterBase(DOUBLE, caller_valP){
			m_value = value;
		}

		Parameter(const char* valueP, uint64_t caller_valP = 0) : ParameterBase(STRING, caller_valP){
			m_value = string(valueP);
		}

		Parameter(const string& value, uint64_t caller_valP = 0) : ParameterBase(STRING, caller_valP){
			m_value = value;
		}

		Parameter(const Parameter& other) : ParameterBase(other.m_type, other.m_caller_valP) {
			m_value = other.m_value;
		}
		Parameter(Parameter&& other) : ParameterBase(other.m_type, other.m_caller_valP)  {
			m_value = other.m_value;
			other.m_type = EMPTY;
		}

		Parameter& operator =(Parameter&& other) {
			*this = move(other);
			return *this;
		}
		Parameter& operator =(const Parameter& other) {
			*this = other;
			return *this;
		}

		/**
		 * \fn Get<Type>Reference
		 * \brief parameter getter, return a reference to the embedded
		 * 		  parameter value such that it can be modified.
		 * \param update_from_ptr if set, the initially passed value 
		 *        is overwritten by the pointed RPCClient side variable
		 *		  which the RPCServer may have modified (out variable).
		*/
		T& GetReference(bool update_from_ptr = false) {
			if (update_from_ptr && m_caller_valP)
				m_value = *((T*)(m_caller_valP));
			return m_value;
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
	void				PrepareSerializeCall(AsyncID asyncId, const string& func_name, vector<unsigned char>& serialized_call, unsigned long* resultP, vector<RemoteProcedureCall::ParameterBase*>* v);
	void 				PrepareSerializeCall(AsyncID asyncId, const string& func_name, vector<unsigned char>& serialized_call, unsigned long* resultP, va_list vl);
	void 				SendSerializedCall(AsyncID asyncId, vector<unsigned char>& serialized_call);

	// rpc function callee side

	vector<ParameterBase*>* DeserializeCall(AsyncID& asyncId, string& func_name);
	void					SerializeCallReturn(AsyncID asyncId, vector<ParameterBase*>* paramP, unsigned long ret_val);

	void Close() {
		if (m_linkP)
			m_linkP->Close();
	}

	static ParamType GetParameterType(const string& type_name) {
		auto t = m_types.find(type_name);
		if (t != m_types.end())
		return t->second;

		return EMPTY;
	}
};

#define ParameterSafeCast(Type, BasePtr) dynamic_cast<RemoteProcedureCall::Parameter<Type>*>(BasePtr)

typedef unsigned long RemoteProcedure(vector<RemoteProcedureCall::ParameterBase*>*, void* user_dataP);

#endif // _RPC_REMOTEPROCEDURECALL_H_
