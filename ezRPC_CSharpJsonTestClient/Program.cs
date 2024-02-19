// See https://aka.ms/new-console-template for more information
using System;
using System.Reflection.Metadata.Ecma335;
using System.Text;
using ezRPC;
using System.Text.Json;

class TestApplication {
	static void Main(string[] args)
	{
		if (args.Length < 3)
		{
			System.Console.WriteLine("usage:");
			System.Console.WriteLine(@" <tcp|file> server_address function_name [repeat_count]");
			System.Console.WriteLine(@" server_address must be addr:port for tcp");
			System.Console.WriteLine(@" tregistered funtions are: nop, inc <int>, repeat <string> <int>,\n\tconcat <string> <int>, sum <int> <int>, inc_double <double>, byebye,\n\tput_string, get_string.");
			return;
		}

		System.Console.WriteLine("# args : {0}", args.Length);
		for (int i = 0; i < args.Length; i++)
			System.Console.WriteLine("	arg #{0} : {1}", i, args[i]);

		string proto = args[0];
		string serverAddr = args[1];
		string functionStr = args[2];

		ulong clientId = JsonClientRpcWrapper.CreateClient(proto == "tcp" ? JsonClientRpcWrapper.TransportType.TCP : JsonClientRpcWrapper.TransportType.FILE, serverAddr);

		JsonCall?  call;
		Parameter[]? parameters;
		if (functionStr == "get_string")
		{
			if (args.Length < 3)
			{
				System.Console.WriteLine(@"usage:");
				System.Console.WriteLine(@" test <tcp|file> server_addr get_string [repeat_count]");
				JsonClientRpcWrapper.DestroyClient(clientId);
				return;
			}

			int repeat = args.Length == 4 ? Int16.Parse(args[3]) : 1;
			for (int i = 0; i < repeat; i++)
			{
				call = new JsonCall
				{
					function = functionStr,
					parameters = new Parameter[] { new Parameter { type = "STRING_REF", value = "overwrite me!" } }
				};

				Semaphore s = new Semaphore(initialCount: 0, maximumCount: 1);
				JsonClientRpcWrapper.AsyncCall(clientId, call, 256, (_asyncId, _call) =>
				{
					if (_call != null)
						Console.WriteLine("asyncId : {0} -> result : {1}", _asyncId, _call.ToJson());
					s.Release();
				});
				s.WaitOne();
			}
		} else if (functionStr == "put_string") {
			if (args.Length < 4)
			{
				System.Console.WriteLine(@"usage:");
				System.Console.WriteLine(@" test <tcp|file> server_addr put_string text [repeat_count]");
				JsonClientRpcWrapper.DestroyClient(clientId);
				return;
			}

			int repeat = args.Length == 5 ? Int16.Parse(args[4]) : 1;
			for (int i = 0; i < repeat; i++)
			{
				call = new JsonCall
				{
					function = functionStr,
					parameters = new Parameter[] {new Parameter { type = "STRING_REF", value = args[3] }}
				};

				Semaphore s = new Semaphore(initialCount: 0, maximumCount: 1);
				JsonClientRpcWrapper.AsyncCall(clientId, call, 256, (_asyncId, _call) =>
				{
					if (_call != null)
						Console.WriteLine("asyncId : {0} -> result : {1}", _asyncId, _call.ToJson());
					s.Release();
				});
				s.WaitOne();
			}
		} else if (functionStr == "nop") {
			if (args.Length < 3)
			{
				System.Console.WriteLine(@"usage:");
				System.Console.WriteLine(@" test <tcp|file> server_addr nop [repeat_count]");
				JsonClientRpcWrapper.DestroyClient(clientId);
				return;
			}

			int repeat = args.Length == 4 ? Int16.Parse(args[3]) : 1;
			for (int i = 0; i < repeat; i++)
			{
				call = new JsonCall
				{
					function = functionStr,
					parameters = new Parameter[] {}
				};
				JsonClientRpcWrapper.Call(clientId, ref call, 256);
				if (call != null)
					Console.WriteLine("result : {0}", call.ToJson());
			}
		} else if (functionStr == "inc") {
			if (args.Length < 4)
			{
				System.Console.WriteLine(@"usage:");
				System.Console.WriteLine(@" test <tcp|file> server_addr inc value [repeat_count]");
				JsonClientRpcWrapper.DestroyClient(clientId);
				return;
			}

			int repeat = args.Length == 5 ? Int16.Parse(args[4]) : 1;
			for (int i = 0; i < repeat; i++)
			{
				call = new JsonCall
				{
					function = functionStr,
					parameters = new Parameter[] { new Parameter { type = "INT16_REF", value = Int16.Parse(args[3]) }}
				};
				JsonClientRpcWrapper.Call(clientId, ref call, 256);
				if (call != null)
					Console.WriteLine("result : {0}", call.ToJson());
			}
		} else if (functionStr == "inc_double") {
			if (args.Length < 4)
			{
				System.Console.WriteLine(@"usage:");
				System.Console.WriteLine(@" test <tcp|file> server_addr inc_double double_value [repeat_count]");
				JsonClientRpcWrapper.DestroyClient(clientId);
				return;
			}

			int repeat = args.Length == 5 ? Int16.Parse(args[4]) : 1;
			for (int i = 0; i < repeat; i++)
			{
				call = new JsonCall
				{
					function = functionStr,
					parameters = new Parameter[] { new Parameter { type = "DOUBLE_REF", value = Double.Parse(args[3]) } }
				};
				JsonClientRpcWrapper.Call(clientId, ref call, 256);
				if (call != null)
					Console.WriteLine("result : {0}", call.ToJson());
			}
		} else if (functionStr == "concat") {
			if (args.Length < 5)
			{
				System.Console.WriteLine(@"usage:");
				System.Console.WriteLine(@" test <tcp|file> server_addr concat string num_concat [repeat_count]");
				JsonClientRpcWrapper.DestroyClient(clientId);
				return;
			}

			int repeat = args.Length == 6 ? Int16.Parse(args[5]) : 1;
			for (int i = 0; i < repeat; i++)
			{
				call = new JsonCall
				{
					function = functionStr,
					parameters = new Parameter[] { new Parameter { type = "STRING_REF", value = args[3] },
												   new Parameter { type = "INT16", value = Int16.Parse(args[4]) } }
				};
				JsonClientRpcWrapper.Call(clientId, ref call, 1024);
				if (call != null)
					Console.WriteLine("result : {0}", call.ToJson());
			}
		} else if (functionStr == "repeat") {
			if (args.Length < 5)
			{
				System.Console.WriteLine(@"usage:");
				System.Console.WriteLine(@" test <tcp|file> server_addr repeat string num_repeat [repeat_count]");
				JsonClientRpcWrapper.DestroyClient(clientId);
				return;
			}

			int repeat = args.Length == 6 ? Int16.Parse(args[5]) : 1;
			for (int i = 0; i < repeat; i++)
			{
				call = new JsonCall
				{
					function = functionStr,
					parameters = new Parameter[] { new Parameter { type = "STRING", value = args[3] },
												   new Parameter { type = "INT16", value = Int16.Parse(args[4]) } }
				};
				JsonClientRpcWrapper.Call(clientId, ref call, 256);
				if (call != null)
					Console.WriteLine("result : {0}", call.ToJson());
			}
		} else if (functionStr == "sum") {
			if (args.Length < 5)
			{
				System.Console.WriteLine(@"usage:");
				System.Console.WriteLine(@" test <tcp|file> server_addr sum number1 number2 [repeat_count]");
				JsonClientRpcWrapper.DestroyClient(clientId);
				return;
			}

			int repeat = args.Length == 6 ? Int16.Parse(args[5]) : 1;
			for (int i = 0; i < repeat; i++)
			{
				call = new JsonCall
				{
					function = functionStr,
					parameters = new Parameter[] { new Parameter { type = "INT16", value = Int16.Parse(args[3]) },
												   new Parameter { type = "INT16", value = Int16.Parse(args[4]) } }
				};
				JsonClientRpcWrapper.Call(clientId, ref call, 256);
				if (call != null)
					Console.WriteLine("result : {0}", call.ToJson());
			}
		} else if (functionStr == "byebye") {
			call = new JsonCall
			{
				function = functionStr,
				parameters = new Parameter[] { }
			};

			JsonClientRpcWrapper.Call(clientId, ref call, 256);
		}

		JsonClientRpcWrapper.DestroyClient(clientId);
	}
}
