
using System.Text.Json;

namespace ezRPC
{
	public class Parameter
	{
		public string? type { get; set; }	  // the parameter type, see RemoteProcedureCall.h for details
		public object? value { get; set; }	  // the passed/returned value	
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
			return JsonSerializer.Deserialize<JsonCall>(jsonString.TrimEnd('\0'));
		}

		/**
		 * \brief Builds a call instance out of a byte[] contained jsonCall serialization. The latter must be properly formed.
		 */
		static public JsonCall? FromJson(byte[] jsonString)
		{
			return JsonCall.FromJson(System.Text.Encoding.ASCII.GetString(jsonString));
		}
	}
}
