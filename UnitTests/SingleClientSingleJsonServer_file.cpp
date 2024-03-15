#include "pch.h"
#include "CppUnitTest.h"

#include <RpcServer.h>
#include <RpcClient.h>
#include <Transport.h>

#include <thread>

#include <JsonRPCServer.h>
#include <nlohmann/json.hpp>

#include "UnitTestsSettings.h"
#include "RemoteProcedures.h"

using json = nlohmann::json;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FILE_RPCtests {

	TEST_CLASS(SingleClientSingleJsonServerTests) {
public:
	static AsyncID			s_asyncId;
	static Semaphore		s_asyncSem;
	static uint64_t			s_asyncResult;

	uint64_t				m_jsonServer;
	unique_ptr<RPCClient>	m_rpcClient;

	static void		ClientAsyncReplyHandler(AsyncID asyncId, uint64_t result);

	SingleClientSingleJsonServerTests() {
		m_jsonServer = CreateRpcServer(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

		RegisterProcedure(m_jsonServer, "Nop", &JsonNop);
		RegisterProcedure(m_jsonServer, "SumNumbers", &JsonSumNumbers);
		RegisterProcedure(m_jsonServer, "IncDouble", &JsonIncDouble);
		RegisterProcedure(m_jsonServer, "Increment", &JsonIncrement);
		RegisterProcedure(m_jsonServer, "Concatenate", &JsonConcatenate);

		thread t([&]() {
			IterateAndWait(m_jsonServer);
			});
		t.detach();
		// server must be ready for incoming connections
		std::this_thread::sleep_for(SERVER_COMM_SETUP_DELAY);

		m_rpcClient = make_unique<RPCClient>(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
	}

	~SingleClientSingleJsonServerTests() {
		Stop(m_jsonServer);
		DestroyRpcServer(m_jsonServer);
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
			RpcReturnValue r = m_rpcClient->RpcCall("SumNumbers",
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

	TEST_METHOD(CallIncrementDoubleAndStop) {
		RPCClient client(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

		double i = 0;
		while (i * 10 < RPC_CALL_ITERATIONS) {
			RpcReturnValue r = m_rpcClient->RpcCall("IncDouble",
				RemoteProcedureCall::PTR,
				RemoteProcedureCall::DOUBLE,
				&i,
				RemoteProcedureCall::END_OF_CALL);
		}

		Assert::IsTrue(i * 10 >= RPC_CALL_ITERATIONS);
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

	AsyncID			SingleClientSingleJsonServerTests::s_asyncId;
	Semaphore		SingleClientSingleJsonServerTests::s_asyncSem(0);
	uint64_t		SingleClientSingleJsonServerTests::s_asyncResult;

	void SingleClientSingleJsonServerTests::ClientAsyncReplyHandler(AsyncID asyncId, uint64_t result) {
		s_asyncId = asyncId;
		s_asyncResult = result;
		s_asyncSem.R();
	}
}
