using System.Runtime.InteropServices;

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
		public static ulong Call(ulong clientId, string jsonCall, ref string jsonCallResult, ulong jsonCallResultLen)
		{
			byte[] jsonResult = new byte[jsonCallResultLen];
			ulong result = RpcCall(clientId, System.Text.Encoding.ASCII.GetBytes(jsonCall), jsonResult, jsonCallResultLen);
			jsonCallResult = new string(System.Text.Encoding.ASCII.GetString(jsonResult));

			return result;
		}
		public static ulong AsyncCall(ulong clientId, string jsonCall, byte[] jsonCallResult, ulong jsonCallResultLen, AsyncJsonReplyProcedureType replyProc)
		{
			return AsyncRpcCall(clientId, System.Text.Encoding.ASCII.GetBytes(jsonCall), jsonCallResult, jsonCallResultLen, replyProc);
		}
		public static void DestroyClient(ulong clientId)
		{
			DestroyRpcClient(clientId);
		}
	}
}
