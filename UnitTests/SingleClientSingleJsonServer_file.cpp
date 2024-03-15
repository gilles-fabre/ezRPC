#include "pch.h"
#include "CppUnitTest.h"

#include <RpcServer.h>
#include <RpcClient.h>
#include <Transport.h>

#include <thread>

#include <JsonRPCServer.h>
#include <nlohmann/json.hpp>

#include "UnitTestsSettings.h"

using json = nlohmann::json;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FILE_RPCtests {

	TEST_CLASS(SingleClientSingleJsonServerTests) {
public:
	static AsyncID			s_asyncId;
	static Semaphore		s_asyncSem;
	static unsigned long	s_asyncResult;

	uint64_t				m_jsonServer;
	unique_ptr<RPCClient>	m_rpcClient;

	static uint64_t Increment(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
	static uint64_t Nop(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
	static uint64_t SumNumbers(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
	static uint64_t Concatenate(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
	static void		ClientAsyncReplyHandler(AsyncID asyncId, unsigned long result);

	SingleClientSingleJsonServerTests() {
		m_jsonServer = CreateRpcServer(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

		RegisterProcedure(m_jsonServer, "Nop", &Nop);
		RegisterProcedure(m_jsonServer, "SumNumbers", &SumNumbers);
		RegisterProcedure(m_jsonServer, "Increment", &Increment);
		RegisterProcedure(m_jsonServer, "Concatenate", &Concatenate);

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

	AsyncID			SingleClientSingleJsonServerTests::s_asyncId;
	Semaphore		SingleClientSingleJsonServerTests::s_asyncSem(0);
	unsigned long	SingleClientSingleJsonServerTests::s_asyncResult;

	uint64_t SingleClientSingleJsonServerTests::SumNumbers(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
		json call = json::parse(jsonCallP);

		int16_t num1 = call["parameters"][1]["value"];
		int16_t num2 = call["parameters"][2]["value"];
		int16_t sum = num1 + num2;

		string jsonCallResult = call.dump();
		if (jsonCallResult.length() < jsonCallResultLen)
			memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

		return (unsigned long)sum;
	}

	uint64_t SingleClientSingleJsonServerTests::Nop(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
		json call = json::parse(jsonCallP);

		string jsonCallResult = call.dump();
		if (jsonCallResult.length() < jsonCallResultLen)
			memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

		return (unsigned long)0;
	}

	uint64_t SingleClientSingleJsonServerTests::Increment(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
		json call = json::parse(jsonCallP);

		int16_t value = call["parameters"][1]["value"];
		value++;
		call["parameters"][1]["value"] = value;

		string jsonCallResult = call.dump();
		if (jsonCallResult.length() < jsonCallResultLen)
			memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

		return (unsigned long)0;
	}

	uint64_t SingleClientSingleJsonServerTests::Concatenate(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
		json call = json::parse(jsonCallP);

		string text = call["parameters"][1]["value"];
		int16_t num = call["parameters"][2]["value"];

		string concatText = text;
		for (int i = 0; i < num; i++)
			concatText += text;

		call["parameters"][1]["value"] = concatText;

		string jsonCallResult = call.dump();
		if (jsonCallResult.length() < jsonCallResultLen)
			memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

		return (unsigned long)concatText.length();
	}

	void SingleClientSingleJsonServerTests::ClientAsyncReplyHandler(AsyncID asyncId, unsigned long result) {
		s_asyncId = asyncId;
		s_asyncResult = result;
		s_asyncSem.R();
	}
}
