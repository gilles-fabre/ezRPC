#include "pch.h"
#include "CppUnitTest.h"

#include <RpcServer.h>
#include <RpcClient.h>
#include <Transport.h>

#include <thread>

#include "UnitTestsSettings.h"
#include "RemoteProcedures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FILE_RPCtests {

	TEST_CLASS(ClientServerPairTests) {
	public:
		static AsyncID			s_asyncId;
		static Semaphore		s_asyncSem;
		static uint64_t			s_asyncResult;
		static RPCServer*		s_rpcServerP;

		static void			 ClientAsyncReplyHandler(AsyncID asyncId, uint64_t result);

		TEST_METHOD(CallNopAndStop) {
			RPCServer server(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			s_rpcServerP = &server;


			thread t([&]() {
				server.RegisterProcedure("Nop", &Nop);
				server.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				RPCClient client(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
				client.RpcCall("Nop", RemoteProcedureCall::END_OF_CALL);
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
			server.Stop();
		}

		TEST_METHOD(ClientErrorIfNoServer) {
			RPCClient client(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			RpcReturnValue r = client.RpcCall("Nop", RemoteProcedureCall::END_OF_CALL);

			Assert::IsTrue(r.IsError());
		}

		TEST_METHOD(ClientErrorIfBadServerAddress) {
			RPCServer server(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			s_rpcServerP = &server;


			thread t([&]() {
				server.RegisterProcedure("Nop", &Nop);
				server.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

			RPCClient client(RPC_TRANSPORT, RPC_BAD_SERVER_ADDRESS);
			RpcReturnValue  r = client.RpcCall("Nop", RemoteProcedureCall::END_OF_CALL);

			Assert::IsTrue(r.IsError());
			server.Stop();
		}

		TEST_METHOD(ClientErrorWhenServerStops) {
			RPCServer server(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			s_rpcServerP = &server;


			thread t([&]() {
				server.RegisterProcedure("Nop", &Nop);
				server.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);


			RPCClient client(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			RpcReturnValue r = client.RpcCall("Nop", RemoteProcedureCall::END_OF_CALL);

			Assert::IsTrue(!r.IsError());

			server.Stop();
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

			r = client.RpcCall("Nop", RemoteProcedureCall::END_OF_CALL);

			Assert::IsTrue(r.IsError());
		}

		TEST_METHOD(CallSumTwoNumbersAndStop) {
			RPCServer server(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			s_rpcServerP = &server;

			server.RegisterProcedure("SumNumbers", &SumNumbers);
			thread t([&]() {
				server.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

			RPCClient client(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			RpcReturnValue r;
			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				r = client.RpcCall("SumNumbers",
					RemoteProcedureCall::INT16,
					1234,
					RemoteProcedureCall::INT16,
					4321,
					RemoteProcedureCall::END_OF_CALL);
				Assert::AreEqual(uint32_t(5555), r.GetResult());
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
			server.Stop();
		}

		TEST_METHOD(MissingArgumentsReturnAnError) {
			RPCServer server(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			s_rpcServerP = &server;

			server.RegisterProcedure("SumNumbers", &SumNumbers);
			thread t([&]() {
				server.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

			RPCClient client(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			RpcReturnValue r;
			r = client.RpcCall("SumNumbers",
				RemoteProcedureCall::INT16,
				4321,
				RemoteProcedureCall::END_OF_CALL);

			Assert::IsTrue(r.IsError());
			Assert::AreEqual(r.GetErrorString(), string(RemoteProcedureErrors::m_errors[RemoteProcedureErrors::ErrorCode::WrongNumberOfArguments]));
			server.Stop();
		}

		TEST_METHOD(NullPtrReturnAnError) {
			RPCServer server(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			s_rpcServerP = &server;

			server.RegisterProcedure("Increment", &Increment);
			thread t([&]() {
				server.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

			RPCClient client(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			RpcReturnValue r = client.RpcCall("Increment",
				RemoteProcedureCall::PTR,
				RemoteProcedureCall::INT16,
				NULL,
				RemoteProcedureCall::END_OF_CALL);

			Assert::IsTrue(r.IsError());
			Assert::AreEqual(r.GetErrorString(), string(RemoteProcedureErrors::m_errors[RemoteProcedureErrors::ErrorCode::NullPointer]));
			server.Stop();
		}

		TEST_METHOD(CallIncrementNumberAndStop) {
			RPCServer server(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			s_rpcServerP = &server;

			server.RegisterProcedure("Increment", &Increment);
			thread t([&]() {
				server.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

			RPCClient client(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			uint16_t i = 0;
			while (i < RPC_CALL_ITERATIONS) {
				RpcReturnValue r = client.RpcCall("Increment",
					RemoteProcedureCall::PTR,
					RemoteProcedureCall::INT16,
					&i,
					RemoteProcedureCall::END_OF_CALL);
			}

			Assert::AreEqual(i, uint16_t(RPC_CALL_ITERATIONS));
			server.Stop();
		}

		TEST_METHOD(CallIncrementDoubleAndStop) {
			RPCServer server(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			s_rpcServerP = &server;

			server.RegisterProcedure("IncDouble", &IncDouble);
			thread t([&]() {
				server.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

			RPCClient client(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			double i = 0;
			while (i * 10 < RPC_CALL_ITERATIONS) {
				RpcReturnValue r = client.RpcCall("IncDouble",
					RemoteProcedureCall::PTR,
					RemoteProcedureCall::DOUBLE,
					&i,
					RemoteProcedureCall::END_OF_CALL);
			}

			Assert::IsTrue(i * 10 >= RPC_CALL_ITERATIONS);
			server.Stop();
		}

		TEST_METHOD(AsyncCallIncrementNumberAndStop) {
			RPCServer server(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			s_rpcServerP = &server;

			server.RegisterProcedure("Increment", &Increment);
			thread t([&]() {
				server.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

			RPCClient client(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			uint16_t i = 0;
			while (i < RPC_CALL_ITERATIONS) {
				AsyncID asyncId = client.RpcCallAsync(ClientAsyncReplyHandler, 
					"Increment",
					RemoteProcedureCall::PTR,
					RemoteProcedureCall::INT16,
					&i,
					RemoteProcedureCall::END_OF_CALL);

				s_asyncSem.A();
				Assert::AreEqual(asyncId, s_asyncId);
			}

			Assert::AreEqual(i, uint16_t(RPC_CALL_ITERATIONS));
			server.Stop();
		}

		TEST_METHOD(AsyncCallSumTwoNumbersAndStop) {
			RPCServer server(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			s_rpcServerP = &server;

			server.RegisterProcedure("SumNumbers", &SumNumbers);
			thread t([&]() {
				server.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

			RPCClient client(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				AsyncID asyncId = client.RpcCallAsync(ClientAsyncReplyHandler,
					"SumNumbers",
					RemoteProcedureCall::INT16,
					1234,
					RemoteProcedureCall::INT16,
					4321,
					RemoteProcedureCall::END_OF_CALL);
				s_asyncSem.A();
				Assert::AreEqual(asyncId, s_asyncId);
				Assert::AreEqual(uint64_t(5555), s_asyncResult);
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
			server.Stop();
		}

		TEST_METHOD(CallConcatTwoStringsAndStop) {
			RPCServer server(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			s_rpcServerP = &server;

			server.RegisterProcedure("Concatenate", &Concatenate);
			thread t([&]() {
				server.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

			RPCClient client(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				string str = "foo";
				client.RpcCall("Concatenate",
					RemoteProcedureCall::PTR,
					RemoteProcedureCall::STRING,
					&str,
					RemoteProcedureCall::INT16,
					2,
					RemoteProcedureCall::END_OF_CALL);

				Assert::AreEqual(str, string("foofoofoo"));
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
			server.Stop();
		}

		TEST_METHOD(AsyncCallConcatTwoStringsAndStop) {
			RPCServer server(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
			s_rpcServerP = &server;

			server.RegisterProcedure("Concatenate", &Concatenate);
			thread t([&]() {
				server.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

			RPCClient client(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				string str = "foo";
				AsyncID asyncId = client.RpcCallAsync(ClientAsyncReplyHandler, 
					"Concatenate",
					RemoteProcedureCall::PTR,
					RemoteProcedureCall::STRING,
					&str,
					RemoteProcedureCall::INT16,
					2,
					RemoteProcedureCall::END_OF_CALL);

				s_asyncSem.A();
				Assert::AreEqual(asyncId, s_asyncId);
				Assert::AreEqual(str, string("foofoofoo"));
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
			server.Stop();
		}
	};

	AsyncID			ClientServerPairTests::s_asyncId;
	Semaphore		ClientServerPairTests::s_asyncSem(0);
	uint64_t		ClientServerPairTests::s_asyncResult;
	RPCServer*		ClientServerPairTests::s_rpcServerP = NULL;

	void ClientServerPairTests::ClientAsyncReplyHandler(AsyncID asyncId, uint64_t result) {
		s_asyncId = asyncId;
		s_asyncResult = result;
		s_asyncSem.R();
	}
}
