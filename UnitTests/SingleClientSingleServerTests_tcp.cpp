#include "pch.h"
#include "CppUnitTest.h"

#include <RpcServer.h>
#include <RpcClient.h>
#include <Transport.h>

#include <thread>

#define TCP
#include "UnitTestsSettings.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TCP_RPCtests {

	TEST_CLASS(SingleClientSingleServerTests) {
	public:
		static AsyncID			s_asyncId;
		static Semaphore		s_asyncSem;
		static unsigned long	s_asyncResult;
		
		RPCServer				m_rpcServer;
		unique_ptr<RPCClient>	m_rpcClient;

		static unsigned long Increment(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
		static unsigned long Nop(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
		static unsigned long SumNumbers(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
		static unsigned long Concatenate(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
		static void			 ClientAsyncReplyHandler(AsyncID asyncId, unsigned long result);

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
			std::this_thread::sleep_for(1s);

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

		TEST_METHOD(CallSumTwoNumbersAndStop) {
			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				unsigned long result = m_rpcClient->RpcCall("SumNumbers",
					RemoteProcedureCall::INT16,
					1234,
					RemoteProcedureCall::INT16,
					4321,
					RemoteProcedureCall::END_OF_CALL);
				Assert::AreEqual(unsigned long(5555), result);
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
		}

		TEST_METHOD(CallIncrementNumberAndStop) {
			uint16_t i = 0;
			while (i < RPC_CALL_ITERATIONS) {
				unsigned long result = m_rpcClient->RpcCall("Increment",
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
				Assert::AreEqual(unsigned long(5555), s_asyncResult);
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
	unsigned long	SingleClientSingleServerTests::s_asyncResult;

	unsigned long SingleClientSingleServerTests::Increment(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
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

	unsigned long SingleClientSingleServerTests::Nop(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
		return 0;
	}

	unsigned long SingleClientSingleServerTests::SumNumbers(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
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

	unsigned long SingleClientSingleServerTests::Concatenate(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
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

	void SingleClientSingleServerTests::ClientAsyncReplyHandler(AsyncID asyncId, unsigned long result) {
		s_asyncId = asyncId;
		s_asyncResult = result;
		s_asyncSem.R();
	}
}
