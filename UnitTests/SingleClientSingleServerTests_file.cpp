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

	TEST_CLASS(SingleClientSingleServerTests) {
	public:
		static AsyncID			s_asyncId;
		static Semaphore		s_asyncSem;
		static uint64_t			s_asyncResult;
		
		RPCServer				m_rpcServer;
		unique_ptr<RPCClient>	m_rpcClient;

		static void			 ClientAsyncReplyHandler(AsyncID asyncId, uint64_t result);

		SingleClientSingleServerTests() : m_rpcServer(RPC_TRANSPORT, RPC_SERVER_ADDRESS) {
			thread t([&]() {
				m_rpcServer.RegisterProcedure("Nop", &Nop);
				m_rpcServer.RegisterProcedure("SumNumbers", &SumNumbers);
				m_rpcServer.RegisterProcedure("Increment", &Increment);
				m_rpcServer.RegisterProcedure("Concatenate", &Concatenate);

				m_rpcServer.IterateAndWait();
				});
			t.detach();
			// server must be ready for incoming connections
			std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

			m_rpcClient = make_unique<RPCClient>(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
		}

		~SingleClientSingleServerTests() {
			m_rpcServer.Stop();
		}

		TEST_METHOD(CallNopAndStop) {
			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				m_rpcClient->RpcCall("Nop", RemoteProcedureCall::END_OF_CALL);
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
		}

		TEST_METHOD(ClientErrorIfBadServerAddress) {
			RPCClient rpcClient(RPC_TRANSPORT, RPC_BAD_SERVER_ADDRESS);
			RpcReturnValue  r = rpcClient.RpcCall("Nop", RemoteProcedureCall::END_OF_CALL);

			Assert::IsTrue(r.IsError());
		}

		TEST_METHOD(MissingArgumentsReturnAnError) {
			RpcReturnValue r;
			r = m_rpcClient->RpcCall("SumNumbers",
				RemoteProcedureCall::INT16,
				4321,
				RemoteProcedureCall::END_OF_CALL);

			Assert::IsTrue(r.IsError());
			Assert::AreEqual(r.GetErrorString(), string(RemoteProcedureErrors::m_errors[RemoteProcedureErrors::ErrorCode::WrongNumberOfArguments]));
		}

		TEST_METHOD(NullPtrReturnAnError) {
			RpcReturnValue r = m_rpcClient->RpcCall("Increment",
				RemoteProcedureCall::PTR,
				RemoteProcedureCall::INT16,
				NULL,
				RemoteProcedureCall::END_OF_CALL);

			Assert::IsTrue(r.IsError());
			Assert::AreEqual(r.GetErrorString(), string(RemoteProcedureErrors::m_errors[RemoteProcedureErrors::ErrorCode::NullPointer]));
		}

		TEST_METHOD(CallSumTwoNumbersAndStop) {
			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				RpcReturnValue r = m_rpcClient->RpcCall("SumNumbers",
					RemoteProcedureCall::INT16,
					1234,
					RemoteProcedureCall::INT16,
					4321,
					RemoteProcedureCall::END_OF_CALL);
				Assert::AreEqual(uint64_t(5555), (uint64_t)r.GetResult());
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
		}

		TEST_METHOD(CallIncrementNumberAndStop) {
			uint16_t i = 0;
			while (i < RPC_CALL_ITERATIONS) {
				RpcReturnValue r = m_rpcClient->RpcCall("Increment",
					RemoteProcedureCall::PTR,
					RemoteProcedureCall::INT16,
					&i,
					RemoteProcedureCall::END_OF_CALL);
			}

			Assert::AreEqual(i, uint16_t(RPC_CALL_ITERATIONS));
		}

		TEST_METHOD(AsyncCallIncrementNumberAndStop) {
			uint16_t i = 0;
			while (i < RPC_CALL_ITERATIONS) {
				AsyncID asyncId = m_rpcClient->RpcCallAsync(ClientAsyncReplyHandler,
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
			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				AsyncID asyncId = m_rpcClient->RpcCallAsync(ClientAsyncReplyHandler,
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
			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				string str = "foo";
				m_rpcClient->RpcCall("Concatenate",
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
			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				string str = "foo";
				AsyncID asyncId = m_rpcClient->RpcCallAsync(ClientAsyncReplyHandler,
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

	AsyncID			SingleClientSingleServerTests::s_asyncId;
	Semaphore		SingleClientSingleServerTests::s_asyncSem(0);
	uint64_t		SingleClientSingleServerTests::s_asyncResult;

	void SingleClientSingleServerTests::ClientAsyncReplyHandler(AsyncID asyncId, uint64_t result) {
		s_asyncId = asyncId;
		s_asyncResult = result;
		s_asyncSem.R();
	}
}
