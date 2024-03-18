
using System.Text.Json;
using System.Text.Json.Serialization;

namespace ezRPC
{
	public enum ErrorCode
	{
		None,
		EmptyResult,
		BadArgument,
		AllocationError,
		MissingData,
		CommunicationDropped,
		BadProtocol,
		SocketCreationError,
		SocketConnectionError,
		InvalidAddress,
		SocketSettingError,
		SocketListeningError,
		ProtocolError,
		RpcParametersError,
		WrongNumberOfArguments,
		NullPointer,
		JsonParsingException,
		JsonResultBufferTooSmall,
		END
	};

	public struct RpcReturnValue
	{
		private uint?		m_value;
		private ErrorCode	m_error;

		public static implicit operator ulong(RpcReturnValue returnValue)
		{
			uint vValue = returnValue.m_value.HasValue ? returnValue.m_value.Value : 0;
			uint eValue = returnValue.m_value.HasValue ? (uint)returnValue.m_error : 0;

			ulong value = eValue;
			value <<= 32;
			value |= vValue;
			return value;
		}

		public static implicit operator RpcReturnValue(ulong value)
		{
			uint eValue = (uint)((value & 0xFFFFFFFF00000000) >> 32);
			uint vValue = (uint)(value & 0xFFFFFFFF);

			return new RpcReturnValue { m_value = vValue, m_error = (ErrorCode)eValue };
		}

		public RpcReturnValue() { m_value = 0; m_error = ErrorCode.EmptyResult; }
		public RpcReturnValue(uint value) { m_value = value; m_error = ErrorCode.None; }
		public RpcReturnValue(ErrorCode error) { m_value = 0; m_error = error; }
		public RpcReturnValue(uint value, ErrorCode error) { m_value = value; m_error = error; }

		public bool IsError() {
			return m_error != ErrorCode.None;
		}
		public uint? GetResult() {
			return m_value;
		}
		public ErrorCode GetError() {
			return m_error;
		}
	}

	public class Parameter
	{
		public string? type { get; set; }	  // the parameter type, see RemoteProcedureCall.h for details
		public object? value { get; set; }    // the passed/returned value	
		[JsonIgnore(Condition = JsonIgnoreCondition.WhenWritingNull)]
		public UInt64? reference { get; set; } // this is set when the caller is NOT a json caller, but a straight RPC client. It contains the (uint64_t)ptr to the client's variable
	}

	/**
	 * This class serves the serialization/deserialization of the call/return parameters
	 */
	public class JsonCall
	{
		public string? function { get; set; }			// the called server's procedure name
		public Parameter[]? parameters { get; set; }	// the associated parameters

		public bool IsValid()
		{
			bool isValid = false;

			if (isValid = !String.IsNullOrEmpty(function) && parameters != null)
			{
				foreach (Parameter p in parameters!)
				{
					isValid &= p != null && !String.IsNullOrEmpty(p.type) && p.value != null;
				}
			}

			return isValid;
		}

		/**
		 * \brief returns a string containing the serialized json corresponding to this call instance
		 */
		public string ToJson()
		{
			if (!IsValid())
				return new string(@"{""error"" : ""invalid call!""}");

			return JsonSerializer.Serialize<JsonCall>(this);
		}

		/**
		 * \brief Builds a call instance out of a string contained jsonCall serialization. The latter must be properly formed.
		 */
		static public JsonCall? FromJson(string jsonString)
		{
			JsonCall? call = null;

			try
			{
				call = JsonSerializer.Deserialize<JsonCall>(jsonString.TrimEnd('\0'));
			} catch {}

			return call;
		}

		/**
		 * \brief Builds a call instance out of a byte[] contained jsonCall serialization. The latter must be properly formed.
		 */
		static public JsonCall? FromJson(byte[] jsonString)
		{
			JsonCall? call = null;

			try
			{
				call = JsonCall.FromJson(System.Text.Encoding.ASCII.GetString(jsonString));
			}
			catch {}

			return call;
		}
	}
}
