﻿using System.Runtime.InteropServices;
using System.Text.Json;

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
public delegate void AsyncJsonReplyProcedureType(ulong asyncId, byte[] jsonResult);

namespace ezRPC
{
	public class JsonRpcWrapper
	{
		public enum TransportType {TCP, FILE};

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern ulong CreateRpcClient(TransportType transport, char[] serverAddr);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern ulong AsyncRpcCall(ulong clientId, byte[] jsonCall, byte[] jsonCallResult, ulong jsonCallResultLen, AsyncJsonReplyProcedureType replyProc);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern ulong RpcCall(ulong clientId, byte[] jsonCall, byte[] jsonCallResult, ulong jsonCallResultLen);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern void DestroyRpcClient(ulong clientId);

		public static ulong CreateClient(TransportType transport, string serverAddr)
		{
			return CreateRpcClient(transport, serverAddr.ToCharArray());
		}
		public static ulong Call(ulong clientId, ref JsonRpcWrapper.JsonCall? call)
		{
			if (call == null || call.parameters == null)
				return 0;

			byte[] jsonResult = new byte[1024];
			ulong result = RpcCall(clientId, System.Text.Encoding.ASCII.GetBytes(call.ToJson()), jsonResult, 1024);
			call = JsonCall.FromJson(jsonResult);

			return result;
		}
		public static ulong AsyncCall(ulong clientId, JsonRpcWrapper.JsonCall call, byte[] jsonCallResult, ulong jsonCallResultLen, AsyncJsonReplyProcedureType replyProc)
		{
			return AsyncRpcCall(clientId, System.Text.Encoding.ASCII.GetBytes(JsonSerializer.Serialize<JsonRpcWrapper.JsonCall>(call)), jsonCallResult, jsonCallResultLen, replyProc);
		}
		public static void DestroyClient(ulong clientId)
		{
			DestroyRpcClient(clientId);
		}

		public class Parameter
		{
			public string? type { get; set; }
			public object? value { get; set; }
		}

		public class JsonCall {
			public string? function { get; set; }
			public Parameter[]? parameters { get; set; }

			public bool IsValid()
			{
				bool isValid = false;

				if (isValid = !String.IsNullOrEmpty(function) && parameters != null)
				{
					foreach (Parameter p in parameters!) {
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
}