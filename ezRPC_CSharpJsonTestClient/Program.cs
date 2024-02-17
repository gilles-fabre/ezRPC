// See https://aka.ms/new-console-template for more information
using System;
using System.Reflection.Metadata.Ecma335;
using System.Text;
using ezRPC;
using System.Text.Json;

class TestApplication {
	static void Main(string[] args)
	{
		ulong clientId = JsonRpcWrapper.CreateClient(JsonRpcWrapper.TransportType.TCP, "127.0.0.1:4444");

		JsonRpcWrapper.JsonCall call = new JsonRpcWrapper.JsonCall { function = "repeat", 
																	 parameters = new JsonRpcWrapper.Parameter[] { 
																		 new JsonRpcWrapper.Parameter { type = "STRING", value = "_pouet" }, 
																		 new JsonRpcWrapper.Parameter { type = "INT16", value = 100 } } };
		JsonRpcWrapper.Call(clientId, ref call);
		Console.WriteLine("result : {0}", call.ToJson());

		call = new JsonRpcWrapper.JsonCall
		{
			function = "concat",
			parameters = new JsonRpcWrapper.Parameter[] {
						 new JsonRpcWrapper.Parameter { type = "STRING_REF", value = "_yop" },
						 new JsonRpcWrapper.Parameter { type = "INT16", value = 100 } }
		};
		JsonRpcWrapper.Call(clientId, ref call);
		Console.WriteLine("result : {0}", call.ToJson());

		call = new JsonRpcWrapper.JsonCall
		{
			function = "inc",
			parameters = new JsonRpcWrapper.Parameter[] {
						 new JsonRpcWrapper.Parameter { type = "INT16", value = 199 } }
		};
		JsonRpcWrapper.Call(clientId, ref call);
		Console.WriteLine("result : {0}", call.ToJson());

		call = new JsonRpcWrapper.JsonCall
		{
			function = "sum",
			parameters = new JsonRpcWrapper.Parameter[] {
						 new JsonRpcWrapper.Parameter { type = "INT16", value = 1234 },
						 new JsonRpcWrapper.Parameter    { type = "INT16", value = 4321 }}
		};
		JsonRpcWrapper.Call(clientId, ref call);
		Console.WriteLine("result : {0}", call.ToJson());

		call = new JsonRpcWrapper.JsonCall
		{
			function = "get_string",
			parameters = new JsonRpcWrapper.Parameter[] {
						 new JsonRpcWrapper.Parameter { type = "STRING_REF", value = "overwrite me!" } }
		};

		byte[] jsonResult = new byte[1024];
		Semaphore s = new Semaphore(initialCount: 0, maximumCount: 1);
		JsonRpcWrapper.AsyncCall(clientId, call, jsonResult, 1024, (_asyncId, _jsonResult) =>
		{
			Console.WriteLine("asyncId : {0} -> result : {1}", _asyncId, new string(System.Text.Encoding.ASCII.GetString(jsonResult)));
			s.Release();
		});
		s.WaitOne();

		call = new JsonRpcWrapper.JsonCall
		{
			function = "inc_double",
			parameters = new JsonRpcWrapper.Parameter[] {
						 new JsonRpcWrapper.Parameter { type = "DOUBLE_REF", value = 1.5 } }
		};

		jsonResult = new byte[1024];
		s = new Semaphore(initialCount: 0, maximumCount: 1);
		JsonRpcWrapper.AsyncCall(clientId, call, jsonResult, 1024, (_asyncId, _jsonResult) =>
		{
			Console.WriteLine("asyncId : {0} -> result : {1}", _asyncId, new string(System.Text.Encoding.ASCII.GetString(jsonResult)));
			s.Release();
		});
		s.WaitOne();

		JsonRpcWrapper.DestroyClient(clientId);
	}
}
