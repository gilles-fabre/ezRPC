using ezRPC;
using System.Runtime.InteropServices;
using System.Text.Json;

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
public delegate void AsyncJsonCallReplyProcedureType(ulong asyncId, JsonCall? call);

namespace ezRPC
{
	public class JsonClientRpcWrapper
	{
		ulong m_clientId {  get; set; }	

		public enum TransportType {TCP, FILE};

		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		public delegate void AsyncJsonReplyProcedureType(ulong asyncId);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern ulong CreateRpcClient(TransportType transport, byte[] serverAddr);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern ulong AsyncRpcCall(ulong clientId, byte[] jsonCall, byte[] jsonCallResult, ulong jsonCallResultLen, AsyncJsonReplyProcedureType replyProc);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern ulong RpcCall(ulong clientId, byte[] jsonCall, byte[] jsonCallResult, ulong jsonCallResultLen);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern void DestroyRpcClient(ulong clientId);

		public void CreateClient(TransportType transport, string serverAddr)
		{
			m_clientId = CreateRpcClient(transport, System.Text.Encoding.ASCII.GetBytes(serverAddr));
		}
		public ulong Call(ref JsonCall? call, ulong maxJsonReplyLen)
		{
			if (m_clientId == 0 || call == null || call.parameters == null)
				return 0;

			byte[] jsonResult = new byte[maxJsonReplyLen + 1];
			ulong result = RpcCall(m_clientId, System.Text.Encoding.ASCII.GetBytes(call.ToJson()), jsonResult, maxJsonReplyLen);
			call = JsonCall.FromJson(jsonResult);

			return result;
		}
		public ulong AsyncCall(JsonCall call, ulong maxJsonReplyLen, AsyncJsonCallReplyProcedureType replyProc)
		{
			if (m_clientId == 0)
				return 0;

			byte[] jsonCallResult = new byte[maxJsonReplyLen + 1];	
			return AsyncRpcCall(m_clientId, System.Text.Encoding.ASCII.GetBytes(JsonSerializer.Serialize<JsonCall>(call)), jsonCallResult, maxJsonReplyLen, (_asyncId) =>
			{
				JsonCall? callResult = JsonCall.FromJson(jsonCallResult);
				replyProc(_asyncId, callResult);
			});
		}
		public void DestroyClient()
		{
			if (m_clientId == 0)
				return;

			DestroyRpcClient(m_clientId);
			m_clientId = 0;	
		}
	}
}
