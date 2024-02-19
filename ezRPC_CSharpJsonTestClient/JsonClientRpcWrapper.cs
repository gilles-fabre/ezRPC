using ezRPC;
using System.Runtime.InteropServices;
using System.Text.Json;

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
public delegate void AsyncJsonCallReplyProcedureType(ulong asyncId, JsonCall? call);

namespace ezRPC
{
	public class JsonClientRpcWrapper
	{
		public enum TransportType {TCP, FILE};

		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		public delegate void AsyncJsonReplyProcedureType(ulong asyncId);

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
		public static ulong Call(ulong clientId, ref JsonCall? call, ulong maxJsonReplyLen)
		{
			if (call == null || call.parameters == null)
				return 0;

			byte[] jsonResult = new byte[maxJsonReplyLen + 1];
			ulong result = RpcCall(clientId, System.Text.Encoding.ASCII.GetBytes(call.ToJson()), jsonResult, maxJsonReplyLen);
			call = JsonCall.FromJson(jsonResult);

			return result;
		}
		public static ulong AsyncCall(ulong clientId, JsonCall call, ulong maxJsonReplyLen, AsyncJsonCallReplyProcedureType replyProc)
		{
			byte[] jsonCallResult = new byte[maxJsonReplyLen + 1];	
			return AsyncRpcCall(clientId, System.Text.Encoding.ASCII.GetBytes(JsonSerializer.Serialize<JsonCall>(call)), jsonCallResult, maxJsonReplyLen, (_asyncId) =>
			{
				JsonCall? callResult = JsonCall.FromJson(jsonCallResult);
				replyProc(_asyncId, callResult);
			});
		}
		public static void DestroyClient(ulong clientId)
		{
			DestroyRpcClient(clientId);
		}
	}
}
