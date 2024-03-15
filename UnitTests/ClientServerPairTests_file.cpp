#include "pch.h"
#include "CppUnitTest.h"

#include <RpcServer.h>
#include <RpcClient.h>
#include <Transport.h>

#include <thread>

#include "UnitTestsSettings.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FILE_RPCtests {

	TEST_CLASS(ClientServerPairTests) {
	public:
		static AsyncID			s_asyncId;
		static Semaphore		s_asyncSem;
		static unsigned long	s_asyncResult;
		static RPCServer*		s_rpcServerP;

		static unsigned long Increment(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
		static unsigned long Nop(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
		static unsigned long SumNumbers(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
		static unsigned long Concatenate(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
		static void			 ClientAsyncReplyHandler(AsyncID asyncId, unsigned long result);

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
			ReturnValue<unsigned long, CommunicationErrors> r = client.RpcCall("Nop", RemoteProcedureCall::END_OF_CALL);

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
			ReturnValue<unsigned long, CommunicationErrors> r = client.RpcCall("Nop", RemoteProcedureCall::END_OF_CALL);

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
			ReturnValue<unsigned long, CommunicationErrors> r = client.RpcCall("Nop", RemoteProcedureCall::END_OF_CALL);

			Assert::AreEqual(r.GetErrorString(), string(CommunicationErrors::m_errors[CommunicationErrors::ErrorCode::None]));
			server.Stop();

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

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				unsigned long result = client.RpcCall("SumNumbers",
					RemoteProcedureCall::INT16,
					1234,
					RemoteProcedureCall::INT16,
					4321,
					RemoteProcedureCall::END_OF_CALL);
				Assert::AreEqual(unsigned long(5555), result);
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
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
				unsigned long result = client.RpcCall("Increment",
					RemoteProcedureCall::PTR,
					RemoteProcedureCall::INT16,
					&i,
					RemoteProcedureCall::END_OF_CALL);
			}

			Assert::AreEqual(i, uint16_t(RPC_CALL_ITERATIONS));
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
				Assert::AreEqual(unsigned long(5555), s_asyncResult);
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
	unsigned long	ClientServerPairTests::s_asyncResult;
	RPCServer*		ClientServerPairTests::s_rpcServerP = NULL;

	unsigned long ClientServerPairTests::Increment(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
		if (v->size() < 2)
			return -1;

		RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
		RemoteProcedureCall::Parameter<int16_t>* p1 = ParameterSafeCast(int16_t, (*v)[1]);


		if (!pReturn || !p1)
			return -1;

		int16_t& i = p1->GetReference();
		++i;

		return (unsigned long)i;
	}

	unsigned long ClientServerPairTests::Nop(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
		return 0;
	}

	unsigned long ClientServerPairTests::SumNumbers(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
		if (v->size() < 3)
			return -1;

		RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
		RemoteProcedureCall::Parameter<int16_t>* p1 = ParameterSafeCast(int16_t, (*v)[1]);
		RemoteProcedureCall::Parameter<int16_t>* p2 = ParameterSafeCast(int16_t, (*v)[2]);

		if (!pReturn || !p1 || !p2)
			return -1;

		int16_t num1 = p1->GetReference();
		int16_t num2 = p2->GetReference();

		return num1 + num2;
	}

	unsigned long ClientServerPairTests::Concatenate(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
		if (v->size() < 3)
			return -1;

		RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
		RemoteProcedureCall::Parameter<string>* p1 = ParameterSafeCast(string, (*v)[1]);
		RemoteProcedureCall::Parameter<int16_t>* p2 = ParameterSafeCast(int16_t, (*v)[2]);

		if (!pReturn || !p1 || !p2)
			return -1;

		string& text = p1->GetReference();
		string origin_text = text;
		int16_t num = p2->GetReference();
		for (int i = 0; i < num; i++)
			text.append(origin_text);

		return (unsigned long)text.length();
	}

	void ClientServerPairTests::ClientAsyncReplyHandler(AsyncID asyncId, unsigned long result) {
		s_asyncId = asyncId;
		s_asyncResult = result;
		s_asyncSem.R();
	}
}
