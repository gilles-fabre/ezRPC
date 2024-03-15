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

	TEST_CLASS(MultiClientSingleServerTests) {
	public:
		static AsyncID			s_asyncId;
		static Semaphore		s_asyncSem;
		static uint64_t			s_asyncResult;
		
		RPCServer		m_rpcServer;

		static void			 ClientAsyncReplyHandler(AsyncID asyncId, uint64_t result);

		MultiClientSingleServerTests() : m_rpcServer(RPC_TRANSPORT, RPC_SERVER_ADDRESS) {
			m_rpcServer.RegisterProcedure("Nop", &Nop);
			m_rpcServer.RegisterProcedure("SumNumbers", &SumNumbers);
			m_rpcServer.RegisterProcedure("Increment", &Increment);
			m_rpcServer.RegisterProcedure("IncDouble", &IncDouble);
			m_rpcServer.RegisterProcedure("Concatenate", &Concatenate);

			thread t([&]() {
				m_rpcServer.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);
		}

		~MultiClientSingleServerTests() {
			m_rpcServer.Stop();
		}

		TEST_METHOD(CallNopAndStop) {
			RPCClient rpcClient(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				rpcClient.RpcCall("Nop", RemoteProcedureCall::END_OF_CALL);
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
		}

		TEST_METHOD(ClientErrorIfBadServerAddress) {
			RPCClient client(RPC_TRANSPORT, RPC_BAD_SERVER_ADDRESS);
			RpcReturnValue  r = client.RpcCall("Nop", RemoteProcedureCall::END_OF_CALL);

			Assert::IsTrue(r.IsError());
		}

		TEST_METHOD(MissingArgumentsReturnAnError) {
			RPCClient rpcClient(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			RpcReturnValue r;
			r = rpcClient.RpcCall("SumNumbers",
				RemoteProcedureCall::INT16,
				4321,
				RemoteProcedureCall::END_OF_CALL);

			Assert::IsTrue(r.IsError());
			Assert::AreEqual(r.GetErrorString(), string(RemoteProcedureErrors::m_errors[RemoteProcedureErrors::ErrorCode::WrongNumberOfArguments]));
		}

		TEST_METHOD(NullPtrReturnAnError) {
			RPCClient rpcClient(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			RpcReturnValue r = rpcClient.RpcCall("Increment",
				RemoteProcedureCall::PTR,
				RemoteProcedureCall::INT16,
				NULL,
				RemoteProcedureCall::END_OF_CALL);

			Assert::IsTrue(r.IsError());
			Assert::AreEqual(r.GetErrorString(), string(RemoteProcedureErrors::m_errors[RemoteProcedureErrors::ErrorCode::NullPointer]));
		}

		TEST_METHOD(CallSumTwoNumbersAndStop) {
			RPCClient rpcClient(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				RpcReturnValue r = rpcClient.RpcCall("SumNumbers",
					RemoteProcedureCall::INT16,
					1234,
					RemoteProcedureCall::INT16,
					4321,
					RemoteProcedureCall::END_OF_CALL);
				Assert::IsTrue(!r.IsError());
				Assert::AreEqual(uint64_t(5555), (uint64_t)r.GetResult());
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
		}

		TEST_METHOD(CallIncrementNumberAndStop) {
			RPCClient rpcClient(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			uint16_t i = 0;
			while (i < RPC_CALL_ITERATIONS) {
				RpcReturnValue r = rpcClient.RpcCall("Increment",
					RemoteProcedureCall::PTR,
					RemoteProcedureCall::INT16,
					&i,
					RemoteProcedureCall::END_OF_CALL);
			}

			Assert::AreEqual(i, uint16_t(RPC_CALL_ITERATIONS));
		}

		TEST_METHOD(CallIncrementDoubleAndStop) {
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
		}

		TEST_METHOD(AsyncCallIncrementNumberAndStop) {
			RPCClient rpcClient(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			uint16_t i = 0;
			while (i < RPC_CALL_ITERATIONS) {
				AsyncID asyncId = rpcClient.RpcCallAsync(ClientAsyncReplyHandler,
					"Increment",
					RemoteProcedureCall::PTR,
					RemoteProcedureCall::INT16,
					&i,
					RemoteProcedureCall::END_OF_CALL);

				s_asyncSem.A();
				Assert::AreEqual(asyncId, s_asyncId);
			}

			Assert::AreEqual(i, uint16_t(RPC_CALL_ITERATIONS));
		}

		TEST_METHOD(AsyncCallSumTwoNumbersAndStop) {
			RPCClient rpcClient(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				AsyncID asyncId = rpcClient.RpcCallAsync(ClientAsyncReplyHandler,
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
		}

		TEST_METHOD(CallConcatTwoStringsAndStop) {
			RPCClient rpcClient(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				string str = "foo";
				rpcClient.RpcCall("Concatenate",
					RemoteProcedureCall::PTR,
					RemoteProcedureCall::STRING,
					&str,
					RemoteProcedureCall::INT16,
					2,
					RemoteProcedureCall::END_OF_CALL);

				Assert::AreEqual(str, string("foofoofoo"));
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
		}

		TEST_METHOD(AsyncCallConcatTwoStringsAndStop) {
			RPCClient rpcClient(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				string str = "foo";
				AsyncID asyncId = rpcClient.RpcCallAsync(ClientAsyncReplyHandler,
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
		}
	};

	AsyncID			MultiClientSingleServerTests::s_asyncId;
	Semaphore		MultiClientSingleServerTests::s_asyncSem(0);
	uint64_t		MultiClientSingleServerTests::s_asyncResult;

	void MultiClientSingleServerTests::ClientAsyncReplyHandler(AsyncID asyncId, uint64_t result) {
		s_asyncId = asyncId;
		s_asyncResult = result;
		s_asyncSem.R();
	}
}
