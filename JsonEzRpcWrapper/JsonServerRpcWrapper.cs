using ezRPC;
using System;
using System.Runtime.InteropServices;
using System.Text.Json;

[UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Ansi)]
public delegate ulong ServerProcedure(ulong asyncId, string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen); 

namespace ezRPC
{
	/**
	* This class is a wrapper to the C++ JsonRPServer API.
	*/
	public class JsonServerRpcWrapper
	{
		ulong m_serverId {  get; set; }	

		public enum TransportType {TCP, FILE};

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern ulong CreateRpcServer(TransportType transport, byte[] serverAddr);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern void RegisterProcedure(ulong serverId, byte[] name, ServerProcedure procedure);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern void UnregisterProcedure(ulong serverId, byte[] name);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern void IterateAndWait(ulong serverId);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern void Stop(ulong serverId);

		[DllImport("ezRPC.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern void DestroyRpcServer(ulong serverId);

		public static void SetUnmanagedAsciizBuffer(string value, IntPtr bufferP) {
			byte[] bytes = new byte[value.Length + 1];
			System.Text.Encoding.ASCII.GetBytes(value).CopyTo(bytes, 0);
			bytes[value.Length] = 0;
			Marshal.Copy(bytes, 0, bufferP, bytes.Length);
		}

		public void CreateServer(TransportType transport, string serverAddr)
		{
			m_serverId = CreateRpcServer(transport, System.Text.Encoding.ASCII.GetBytes(serverAddr));
		}
		public void RegisterProcedure(string name, ServerProcedure procedure)
		{
			if (m_serverId != 0)
				RegisterProcedure(m_serverId, System.Text.Encoding.ASCII.GetBytes(name), procedure);
		}
		public void UnregisterProcedure(string name)
		{
			if (m_serverId != 0)
				UnregisterProcedure(m_serverId, System.Text.Encoding.ASCII.GetBytes(name));
		}
		public void IterateAndWait()
		{
			if (m_serverId != 0)
				IterateAndWait(m_serverId);
		}
		public void Stop()
		{
			if (m_serverId != 0)
				Stop(m_serverId);
		}
		public void DestroyServer()
		{
			if (m_serverId != 0)
				DestroyRpcServer(m_serverId);
			m_serverId = 0;
		}
	}
}
