using System;
using System.Reflection.Metadata.Ecma335;
using System.Text;
using ezRPC;
using System.Text.Json;
using System.Xml.Linq;
using System.Drawing;
using System.Runtime.InteropServices;
using System.ComponentModel.DataAnnotations;
using static System.Net.Mime.MediaTypeNames;

class TestApplication
{
	static JsonServerRpcWrapper? g_jsonServer;

	static ulong ByeBye(ulong asyncId, string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		RpcReturnValue r = new RpcReturnValue();

		if (jsonCallResultP == IntPtr.Zero)
			r = new RpcReturnValue(ErrorCode.NullPointer);
		else
		{
			g_jsonServer!.Stop();
		}
		return r;
	}

	static ulong Nop(ulong asyncId, string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		RpcReturnValue r = new RpcReturnValue();

		if (jsonCallResultP == IntPtr.Zero)
			r = new RpcReturnValue(ErrorCode.NullPointer);
		else try
		{
			JsonCall? call = JsonCall.FromJson(jsonCall);
			if (call != null && call.parameters != null && call.parameters.Length == 1)
			{
				string resultJson = call.ToJson();
				if (resultJson.Length < (int)jsonCallResultLen)
				{
					JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);
					r = new RpcReturnValue((uint)0);
				}
				else
				{
					r = new RpcReturnValue(ErrorCode.JsonResultBufferTooSmall);
				}
				return r;
			}
		} catch {} 

		r = new RpcReturnValue(ErrorCode.BadArgument);	
		return 0;
	}

	static ulong IncrementDouble(ulong asyncId, string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		RpcReturnValue r = new RpcReturnValue();

		if (jsonCallResultP == IntPtr.Zero)
			r = new RpcReturnValue(ErrorCode.NullPointer);
		else try
		{
			JsonCall? call = JsonCall.FromJson(jsonCall);
			if (call != null && call.parameters != null && call.parameters.Length == 2)
			{
				Parameter p = call.parameters[1]!;
				if (p.value != null)
				{
					string? s = p.value.ToString();
					if (s != null)
					{
						double num = double.Parse(s);

						num += 0.1;
						call.parameters[1].value = num;
						string resultJson = call.ToJson();
						if (resultJson.Length < (int)jsonCallResultLen)
						{
							JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);
							r = new RpcReturnValue((uint)0);
						}
						else
						{
							r = new RpcReturnValue(ErrorCode.JsonResultBufferTooSmall);
						}
						return r;
					}
				}
			}
		}
		catch {}

		r = new RpcReturnValue(ErrorCode.BadArgument);
		return r;
	}

	static ulong Increment(ulong asyncId, string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		RpcReturnValue r = new RpcReturnValue();

		if (jsonCallResultP == IntPtr.Zero)
			r = new RpcReturnValue(ErrorCode.NullPointer);
		else try
		{
			JsonCall? call = JsonCall.FromJson(jsonCall);
			if (call != null && call.parameters != null && call.parameters.Length == 2)
			{
				Parameter p = call.parameters[1]!;
				if (p.value != null)
				{
					string? s = p.value.ToString();
					if (s != null)
					{
						short num = short.Parse(s);

						++num;
						call.parameters[1].value = num;
						string resultJson = call.ToJson();
						if (resultJson.Length < (int)jsonCallResultLen) { 
							JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);
							r = new RpcReturnValue((uint)num);
						}
						else
						{
							r = new RpcReturnValue(ErrorCode.JsonResultBufferTooSmall);
						}
						return r;
					}
				}
			}
		}
		catch {}

		r = new RpcReturnValue(ErrorCode.BadArgument);
		return r;
	}

	static ulong Concatenate(ulong asyncId, string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		RpcReturnValue r = new RpcReturnValue();

		if (jsonCallResultP == IntPtr.Zero)
			r = new RpcReturnValue(ErrorCode.NullPointer);
		else try
		{
			JsonCall? call = JsonCall.FromJson(jsonCall);
			if (call != null && call.parameters != null && call.parameters.Length == 3)
			{
				Parameter p1 = call.parameters[1]!;
				Parameter p2 = call.parameters[2]!;
				if (p1.value != null && p2.value != null)
				{
					string? text = p1.value.ToString();
					string? s = p2.value.ToString();
					if (text != null && s != null)
					{
						short num = short.Parse(s);

						string concatText = text;
						for (short i = 0; i < num; i++)
							concatText += text;

						call.parameters[1].value = concatText;

						string resultJson = call.ToJson();
						if (resultJson.Length < (int)jsonCallResultLen) { 
							JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);
							r = new RpcReturnValue((uint)concatText.Length);
						}
						else
						{
							r = new RpcReturnValue(ErrorCode.JsonResultBufferTooSmall);
						}
						return r;
					}
				}
			}
		}
		catch { }

		r = new RpcReturnValue(ErrorCode.BadArgument);
		return r;
	}

	static ulong RepeatPrint(ulong asyncId, string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		RpcReturnValue r = new RpcReturnValue();

		if (jsonCallResultP == IntPtr.Zero)
			r = new RpcReturnValue(ErrorCode.NullPointer);
		else try
		{
			JsonCall? call = JsonCall.FromJson(jsonCall);
			if (call != null && call.parameters != null && call.parameters.Length == 3)
			{
				Parameter p1 = call.parameters[1]!;
				Parameter p2 = call.parameters[2]!;
				if (p1.value != null && p2.value != null)
				{
					string? text = p1.value.ToString();
					string? s = p2.value.ToString();
					if (text != null && s != null)
					{
						short num = short.Parse(s);

						for (short i = 0; i < num; i++)
							Console.WriteLine(text);

						string resultJson = call.ToJson();
						if (resultJson.Length < (int)jsonCallResultLen) { 
							JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);
							r = new RpcReturnValue((uint)0);
						}
						else
						{
							r = new RpcReturnValue(ErrorCode.JsonResultBufferTooSmall);
						}
						return r;
					}
				}
			}
		}
		catch {}

		r = new RpcReturnValue(ErrorCode.BadArgument);
		return r;
	}

	static ulong SumNumbers(ulong asyncId, string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		RpcReturnValue r = new RpcReturnValue();

		if (jsonCallResultP == IntPtr.Zero)
			r = new RpcReturnValue(ErrorCode.NullPointer);
		else try
		{
			JsonCall? call = JsonCall.FromJson(jsonCall);
			if (call != null && call.parameters != null && call.parameters.Length == 3)
			{
				Parameter p1 = call.parameters[1]!;
				Parameter p2 = call.parameters[2]!;
				if (p1.value != null && p2.value != null)
				{
					string? s1 = p1.value.ToString();
					string? s2 = p2.value.ToString();
					if (s1 != null && s2 != null)
					{
						short num1 = short.Parse(s1);
						short num2 = short.Parse(s2);

						Console.WriteLine("{0}", num1 + num2);

						string resultJson = call.ToJson();
						if (resultJson.Length < (int)jsonCallResultLen) { 
							JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);
							r = new RpcReturnValue((uint)(num1 + num2));
						}
						else
						{
							r = new RpcReturnValue(ErrorCode.JsonResultBufferTooSmall);
						}
						return r;
					}
				}
			}
		}
		catch {}

		r = new RpcReturnValue(ErrorCode.BadArgument);
		return r;
	}

	static ulong GetString(ulong asyncId, string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		RpcReturnValue r = new RpcReturnValue();

		if (jsonCallResultP == IntPtr.Zero)
			r = new RpcReturnValue(ErrorCode.NullPointer);
		else try
		{
			JsonCall? call = JsonCall.FromJson(jsonCall);
			if (call != null && call.parameters != null && call.parameters.Length == 2)
			{
				Parameter p1 = call.parameters[1]!;
				if (p1.value != null)
				{
					string? s = p1.value.ToString();
					if (s != null)
					{
						Console.WriteLine("enter a string: ");
						string? text = Console.ReadLine();
						if (text != null)
						{
							call.parameters[1].value = text;
							string resultJson = call.ToJson();
							if (resultJson.Length < (int)jsonCallResultLen) { 
								JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);
								r = new RpcReturnValue((uint)text.Length);
							}
							else
							{
								r = new RpcReturnValue(ErrorCode.JsonResultBufferTooSmall);
							}
							return r;
						}
					}
				}
			}
		}
		catch {}

		r = new RpcReturnValue(ErrorCode.BadArgument);
		return r;
	}

	static ulong PutString(ulong asyncId, string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		RpcReturnValue r = new RpcReturnValue();

		if (jsonCallResultP == IntPtr.Zero)
			r = new RpcReturnValue(ErrorCode.NullPointer);
		else try
		{
			JsonCall? call = JsonCall.FromJson(jsonCall);
			if (call != null && call.parameters != null && call.parameters.Length == 2)
			{
				Parameter p1 = call.parameters[1]!;
				if (p1.value != null)
				{
					string? s = p1.value.ToString();
					if (s != null)
					{
						Console.WriteLine("string passed : {0}", call.parameters[1].value);

						string resultJson = call.ToJson();
						if (resultJson.Length < (int)jsonCallResultLen) { 
							JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);
							r = new RpcReturnValue((uint)0);
						}
						else
						{
							r = new RpcReturnValue(ErrorCode.JsonResultBufferTooSmall);
						}
						return r;
					}
				}
			}
		}
		catch {}

		r = new RpcReturnValue(ErrorCode.BadArgument);
		return r;
	}

	static void Main(string[] args)
	{
		if (args.Length < 2)
		{
			System.Console.WriteLine("usage:");
			System.Console.WriteLine(@" <tcp|file> server_address");
			System.Console.WriteLine(@" server_address must be addr:port for tcp");
			return;
		}

		System.Console.WriteLine("# args : {0}", args.Length);
		for (int i = 0; i < args.Length; i++)
			System.Console.WriteLine("	arg #{0} : {1}", i, args[i]);

		string proto = args[0];
		string serverAddr = args[1];

		g_jsonServer = new JsonServerRpcWrapper();

		g_jsonServer.CreateServer(proto == "tcp" ? JsonServerRpcWrapper.TransportType.TCP : JsonServerRpcWrapper.TransportType.FILE, serverAddr);

		g_jsonServer.RegisterProcedure("nop", Nop);
		g_jsonServer.RegisterProcedure("byebye", ByeBye);
		g_jsonServer.RegisterProcedure("inc_double", IncrementDouble);
		g_jsonServer.RegisterProcedure("inc", Increment);
		g_jsonServer.RegisterProcedure("concat", Concatenate);
		g_jsonServer.RegisterProcedure("repeat", RepeatPrint);
		g_jsonServer.RegisterProcedure("sum", SumNumbers);
		g_jsonServer.RegisterProcedure("get_string", GetString);
		g_jsonServer.RegisterProcedure("put_string", PutString);

		g_jsonServer.IterateAndWait();
	}
};


