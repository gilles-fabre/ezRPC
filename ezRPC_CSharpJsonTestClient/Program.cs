// See https://aka.ms/new-console-template for more information
using System;
using System.Text;
using ezRPC;

class TestApplication {
	static void Main(string[] args)
	{
		ulong clientId = JsonRpcWrapper.CreateClient(JsonRpcWrapper.TransportType.TCP, "127.0.0.1:4444");

		string result = new string("");
		JsonRpcWrapper.Call(clientId, "{\"function\" : \"repeat\", \"parameters\" : [{\"type\" : \"STRING\", \"value\" : \"_pouet\"}, {\"type\" : \"INT16\", \"value\" : 100}] }", ref result, 1024);
		Console.WriteLine("result : {0}", result);

		result = new string("");
		JsonRpcWrapper.Call(clientId, "{\"function\" : \"concat\", \"parameters\" : [{\"type\" : \"STRING_REF\", \"value\" : \"_pouet\"}, {\"type\" : \"INT16\", \"value\" : 100}] }", ref result, 1024);
		Console.WriteLine("result : {0}", result);

		result = new string("");
		JsonRpcWrapper.Call(clientId, "{\"function\" : \"get_string\", \"parameters\" : [{\"type\" : \"STRING_REF\", \"value\" : \"overwrite me!\"}] }", ref result, 1024);
		Console.WriteLine("result : {0}", result);

		byte[] jsonResult = new byte[1024];
		Semaphore s = new Semaphore(initialCount: 0, maximumCount : 1);
		JsonRpcWrapper.AsyncCall(clientId, "{\"function\" : \"get_string\", \"parameters\" : [{\"type\" : \"STRING_REF\", \"value\" : \"overwrite me!\"}] }", jsonResult, 1024, (_asyncId, _jsonResult) =>
		{
			Console.WriteLine("asyncId : {0} -> result : {1}", _asyncId, new string(System.Text.Encoding.ASCII.GetString(jsonResult)));
			s.Release(1);
		});
		s.WaitOne();

		jsonResult = new byte[100];
		JsonRpcWrapper.AsyncCall(clientId, "{\"function\" : \"inc_double\", \"parameters\" : [{\"type\" : \"DOUBLE_REF\", \"value\" : 1.5}] }", jsonResult, 100, (_asyncId, _jsonResult) =>
		{
			Console.WriteLine("asyncId : {0} -> result : {1}", _asyncId, new string(System.Text.Encoding.ASCII.GetString(jsonResult)));
			s.Release(1);
		});
		s.WaitOne();

		JsonRpcWrapper.DestroyClient(clientId);
	}
}
