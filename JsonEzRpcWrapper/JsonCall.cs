
using System.Text.Json;

namespace ezRPC
{
	public class Parameter
	{
		public string? type { get; set; }
		public object? value { get; set; }
	}

	public class JsonCall
	{
		public string? function { get; set; }
		public Parameter[]? parameters { get; set; }

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

		public string ToJson()
		{
			if (!IsValid())
				return new string(@"{""error"" : ""invalid call!""}");

			return JsonSerializer.Serialize<JsonCall>(this);
		}
		static public JsonCall? FromJson(string jsonString)
		{
			return JsonSerializer.Deserialize<JsonCall>(jsonString.TrimEnd('\0'));
		}
		static public JsonCall? FromJson(byte[] jsonString)
		{
			return JsonCall.FromJson(System.Text.Encoding.ASCII.GetString(jsonString));
		}
	}
}
