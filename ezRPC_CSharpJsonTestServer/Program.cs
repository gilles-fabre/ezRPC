using System;
using System.Reflection.Metadata.Ecma335;
using System.Text;
using ezRPC;
using System.Text.Json;
using System.Xml.Linq;
using System.Drawing;
using System.Runtime.InteropServices;
using System.ComponentModel.DataAnnotations;

class TestApplication
{
	static JsonServerRpcWrapper? g_jsonServer;

	static ulong ByeBye(string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		g_jsonServer!.Stop();

		return 0;
	}

	static ulong Nop(string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		JsonCall? call = JsonCall.FromJson(jsonCall);
		if (call != null && call.parameters != null && call.parameters.Length == 1)
		{
			string resultJson = call.ToJson();
			if (resultJson.Length < (int)jsonCallResultLen)
				JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);
		}

		return 0;
	}

	static ulong IncrementDouble(string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		JsonCall? call = JsonCall.FromJson(jsonCall);
		if (call != null && call.parameters != null && call.parameters.Length == 2 && call.parameters[1].value != null)
		{
#pragma warning disable CS8602 // Possible null reference argument.
#pragma warning disable CS8604 // Possible null reference argument.
			double num = double.Parse(call.parameters[1].value.ToString());
#pragma warning restore CS8604 // Possible null reference argument.
#pragma warning restore CS8602 // Possible null reference argument.

			num += 0.1;
			call.parameters[1].value = num;
			string resultJson = call.ToJson();
			if (resultJson.Length < (int)jsonCallResultLen)
				JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);
		}

		return 0;
	}

	static ulong Increment(string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		JsonCall? call = JsonCall.FromJson(jsonCall);
		if (call != null && call.parameters != null && call.parameters.Length == 2 && call.parameters[1].value != null)
		{
#pragma warning disable CS8602 // Possible null reference argument.
#pragma warning disable CS8604 // Possible null reference argument.
			short num = short.Parse(call.parameters[1].value.ToString());
#pragma warning restore CS8604 // Possible null reference argument.
#pragma warning restore CS8602 // Possible null reference argument.

			++num;
			call.parameters[1].value = num;
			string resultJson = call.ToJson();
			if (resultJson.Length < (int)jsonCallResultLen)
				JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);
		}

		return 0;
	}

	static ulong Concatenate(string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		JsonCall? call = JsonCall.FromJson(jsonCall);
		if (call != null && call.parameters != null && call.parameters.Length == 3 && call.parameters[1].value != null && call.parameters[2].value != null)
		{
#pragma warning disable CS8600 // Possible null reference argument.
#pragma warning disable CS8602 // Possible null reference argument.
#pragma warning disable CS8604 // Possible null reference argument.
			string text = call.parameters[1].value.ToString();
			short num = short.Parse(call.parameters[2].value.ToString());

			string concatText = text;
			for (short i = 0; i < num; i++)
				concatText += text;

			call.parameters[1].value = concatText;

			string resultJson = call.ToJson();
			if (resultJson.Length < (int)jsonCallResultLen)
				JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);

			return (ulong)concatText.Length;
#pragma warning restore CS8604 // Possible null reference argument.
#pragma warning restore CS8602 // Possible null reference argument.
#pragma warning restore CS8600 // Possible null reference argument.

		}

		return (ulong)0;
	}

	static ulong RepeatPrint(string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		JsonCall? call = JsonCall.FromJson(jsonCall);
		if (call != null && call.parameters != null && call.parameters.Length == 3 && call.parameters[1].value != null && call.parameters[2].value != null)
		{
#pragma warning disable CS8600 // Possible null reference argument.
#pragma warning disable CS8602 // Possible null reference argument.
#pragma warning disable CS8604 // Possible null reference argument.
			string text = call.parameters[1].value.ToString();
			short num = short.Parse(call.parameters[2].value.ToString());
#pragma warning restore CS8604 // Possible null reference argument.
#pragma warning restore CS8602 // Possible null reference argument.
#pragma warning restore CS8600 // Possible null reference argument.

			for (short i = 0; i < num; i++) 
				Console.WriteLine(text);

			string resultJson = call.ToJson();
			if (resultJson.Length < (int)jsonCallResultLen)
				JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);

			return (ulong)num;
		}

		return (ulong)0;
	}

	static ulong SumNumbers(string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		JsonCall? call = JsonCall.FromJson(jsonCall);
		if (call != null && call.parameters != null && call.parameters.Length == 3 && call.parameters[1].value != null && call.parameters[2].value != null)
		{
#pragma warning disable CS8602 // Possible null reference argument.
#pragma warning disable CS8604 // Possible null reference argument.
			short num1 = short.Parse(call.parameters[1].value.ToString());
			short num2 = short.Parse(call.parameters[2].value.ToString());
#pragma warning restore CS8604 // Possible null reference argument.
#pragma warning restore CS8602 // Possible null reference argument.

			Console.WriteLine("{0}", num1 + num2);

			string resultJson = call.ToJson();
			if (resultJson.Length < (int)jsonCallResultLen)
				JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);
		}

		return 0;
	}

	static ulong GetString(string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		JsonCall? call = JsonCall.FromJson(jsonCall);
		if (call != null && call.parameters != null && call.parameters.Length == 2 && call.parameters[1].value != null)
		{
			Console.WriteLine("enter a string: ");
			string? text = Console.ReadLine();
			if (text != null)
			{
#pragma warning disable CS8602 // Possible null reference argument.
#pragma warning disable CS8604 // Possible null reference argument.
				call.parameters[1].value = text;
#pragma warning restore CS8604 // Possible null reference argument.
#pragma warning restore CS8602 // Possible null reference argument.
				string resultJson = call.ToJson();
				if (resultJson.Length < (int)jsonCallResultLen)
					JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);

				return (ulong)text.Length;
			}
		}

		return 0;
	}

	static ulong PutString(string jsonCall, IntPtr jsonCallResultP, ulong jsonCallResultLen)
	{
		JsonCall? call = JsonCall.FromJson(jsonCall);
		if (call != null && call.parameters != null && call.parameters.Length == 2 && call.parameters[1].value != null) {
#pragma warning disable CS8602 // Possible null reference argument.
#pragma warning disable CS8604 // Possible null reference argument.
			Console.WriteLine("string passed : {0}", call.parameters[1].value);
#pragma warning restore CS8604 // Possible null reference argument.
#pragma warning restore CS8602 // Possible null reference argument.

			string resultJson = call.ToJson();
			if (resultJson.Length < (int)jsonCallResultLen)
				JsonServerRpcWrapper.SetUnmanagedAsciizBuffer(resultJson, jsonCallResultP);
		}

		return 0;
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


