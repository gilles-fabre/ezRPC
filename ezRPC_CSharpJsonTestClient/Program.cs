// See https://aka.ms/new-console-template for more information
using System;
using System.Reflection.Metadata.Ecma335;
using System.Text;
using ezRPC;
using System.Text.Json;

class TestApplication {
	static void Main(string[] args)
	{
		ulong clientId = JsonClientRpcWrapper.CreateClient(JsonClientRpcWrapper.TransportType.TCP, "127.0.0.1:4444");

		JsonCall? call = new JsonCall { function = "repeat", 
										parameters = new Parameter[] { 
													 new Parameter { type = "STRING", value = "_pouet" }, 
													 new Parameter { type = "INT16", value = 100 } } };
		JsonClientRpcWrapper.Call(clientId, ref call, 100);
		if (call != null)
			Console.WriteLine("result : {0}", call.ToJson());

		call = new JsonCall
		{
			function = "concat",
			parameters = new Parameter[] {
						 new Parameter { type = "STRING_REF", value = "_yop" },
						 new Parameter { type = "INT16", value = 100 } }
		};
		JsonClientRpcWrapper.Call(clientId, ref call, 1024);
		if (call != null)
			Console.WriteLine("result : {0}", call.ToJson());

		call = new JsonCall
		{
			function = "inc",
			parameters = new Parameter[] {
						 new Parameter { type = "INT16", value = 199 } }
		};
		JsonClientRpcWrapper.Call(clientId, ref call, 100);
		if (call != null)
			Console.WriteLine("result : {0}", call.ToJson());

		call = new JsonCall
		{
			function = "sum",
			parameters = new Parameter[] {
						 new Parameter { type = "INT16", value = 1234 },
						 new Parameter    { type = "INT16", value = 4321 }}
		};
		JsonClientRpcWrapper.Call(clientId, ref call, 100);
		if (call != null)
			Console.WriteLine("result : {0}", call.ToJson());

		call = new JsonCall
		{
			function = "get_string",
			parameters = new Parameter[] {
						 new Parameter { type = "STRING_REF", value = "overwrite me!" } }
		};

		Semaphore s = new Semaphore(initialCount: 0, maximumCount: 1);
		JsonClientRpcWrapper.AsyncCall(clientId, call, 256, (_asyncId, _call) =>
		{
			if (_call != null)
				Console.WriteLine("asyncId : {0} -> result : {1}", _asyncId, _call.ToJson());
			s.Release();
		});
		s.WaitOne();

		call = new JsonCall
		{
			function = "inc_double",
			parameters = new Parameter[] {
						 new Parameter { type = "DOUBLE_REF", value = 1.5 } }
		};

		s = new Semaphore(initialCount: 0, maximumCount: 1);
		JsonClientRpcWrapper.AsyncCall(clientId, call, 100, (_asyncId, _call) =>
		{
			if (_call != null)
				Console.WriteLine("asyncId : {0} -> result : {1}", _asyncId, _call.ToJson());
			s.Release();
		});
		s.WaitOne();

		JsonClientRpcWrapper.DestroyClient(clientId);
	}
}
