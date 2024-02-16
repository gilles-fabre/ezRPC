// See https://aka.ms/new-console-template for more information
using System;
using ezRPC;

class TestApplication {
	static void Main(string[] args)
	{
		ulong clientId = JsonRpcWrapper.CreateClient(JsonRpcWrapper.TransportType.TCP, "127.0.0.1:4444");

		string result = string.Empty;
		JsonRpcWrapper.Call(clientId, "{\"function\" : \"repeat\", \"parameters\" : [{\"type\" : \"STRING\", \"value\" : \"_pouet\"}, {\"type\" : \"INT16\", \"value\" : 100}] }", ref result);

		JsonRpcWrapper.DestroyClient(clientId);
	}
}
