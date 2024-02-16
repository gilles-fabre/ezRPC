using System.Runtime.InteropServices;

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
public delegate void AsyncJsonReplyProcedureType(ulong asyncId, char[] jsonResult);

namespace ezRPC
{
	public class JsonRpcWrapper
	{
		public enum TransportType {TCP, FILE};

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern ulong CreateRpcClient(TransportType transport, char[] serverAddrP);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern ulong AsyncRpcCall(ulong clientId, char[] jsonCallP, char[] jsonCallResultP, ulong jsonCallResultLen, AsyncJsonReplyProcedureType replyProcP);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern ulong RpcCall(ulong clientId, char[] jsonCallP, char[] jsonCallResultP, ulong jsonCallResultLen);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern void DestroyRpcClient(ulong clientId);

		public static ulong CreateClient(TransportType transport, string serverAddr)
		{
			return CreateRpcClient(transport, serverAddr.ToCharArray());
		}
		public static ulong Call(ulong clientId, string jsonCall, ref string jsonCallResult)
		{
			char[] jsonResult = new char[1024];
			ulong result = RpcCall(clientId, jsonCall.ToCharArray(), jsonResult, 1024);
			jsonCallResult = new string(jsonResult);

			return result;
		}
		public static ulong AsyncCall(ulong clientId, string jsonCall, ref string jsonCallResult)
		{
			char[] jsonResult = new char[1024];
			return AsyncRpcCall(clientId, jsonCall.ToCharArray(), jsonResult, 1024, (asyncId, jsonResult) => { 
				Console.WriteLine("asyncId : {0} -> result : {1}", asyncId, jsonResult);
				//jsonCallResult = new string(jsonResult);
			});
		}
		public static void DestroyClient(ulong clientId)
		{
			DestroyRpcClient(clientId);
		}
	}
}
