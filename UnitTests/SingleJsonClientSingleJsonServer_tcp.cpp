#include "pch.h"
#include "CppUnitTest.h"

#include <RpcServer.h>
#include <RpcClient.h>
#include <Transport.h>

#include <thread>

#include <JsonRPCServer.h>
#include <JsonRPCClient.h>

#include <nlohmann/json.hpp>

#define TCP
#include "UnitTestsSettings.h"

using json = nlohmann::json;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TCP_RPCtests {

	TEST_CLASS(SingleJsonClientSingleJsonServerTests) {
	public:
		static AsyncID			s_asyncId;
		static unsigned long	s_result;
		static Semaphore		s_asyncSem;
		static unsigned long	s_asyncResult;
		static char				s_buffer[JSON_BUFFER_SIZE];
		uint64_t				m_jsonServer;
		uint64_t				m_jsonClient;

		static uint64_t Increment(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
		static uint64_t Nop(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
		static uint64_t SumNumbers(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
		static uint64_t Concatenate(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
		static void		ClientAsyncReplyHandler(AsyncID asyncId, unsigned long result);

		SingleJsonClientSingleJsonServerTests() {
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

			m_jsonClient = CreateRpcClient(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
		}

		~SingleJsonClientSingleJsonServerTests() {
			Stop(m_jsonServer);
			DestroyRpcServer(m_jsonServer);
			DestroyRpcClient(m_jsonClient);
		}

		TEST_METHOD(CallNopAndStop) {
			json call;
			call["function"] = "Nop";

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				RpcCall(m_jsonClient, call.dump().c_str(), s_buffer, sizeof(s_buffer));
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
		}

		TEST_METHOD(CallSumTwoNumbersAndStop) {
			json call;
			call["function"] = "SumNumbers";
			json parameters = json::array();
			{
				json param;
				param["type"] = "INT16";
				param["value"] = 1234;
				parameters += param;
			}
			{
				json param;
				param["type"] = "INT16";
				param["value"] = 4321;
				parameters += param;
			}
			call["parameters"] = parameters;

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				unsigned long result = (unsigned long)RpcCall(m_jsonClient, call.dump().c_str(), s_buffer, sizeof(s_buffer));
				Assert::AreEqual(unsigned long(5555), result);
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
		}

		TEST_METHOD(CallIncrementNumberAndStop) {
			uint16_t i = 0;
			while (i < RPC_CALL_ITERATIONS) {
				json call;
				call["function"] = "Increment";
				json parameters = json::array();
				{
					json param;
					param["type"] = "INT16_REF";
					param["value"] = i;
					parameters += param;
				}
				call["parameters"] = parameters;
				i = (uint16_t)RpcCall(m_jsonClient, call.dump().c_str(), s_buffer, sizeof(s_buffer));
			}

			Assert::AreEqual(i, uint16_t(RPC_CALL_ITERATIONS));
		}

		TEST_METHOD(AsyncCallIncrementNumberAndStop) {
			uint16_t i = 0;
			while (++i < RPC_CALL_ITERATIONS) {
				json call;
				call["function"] = "Increment";
				json parameters = json::array();
				{
					json param;
					param["type"] = "INT16_REF";
					param["value"] = i;
					parameters += param;
				}
				call["parameters"] = parameters;
				AsyncID asyncId = RpcCallAsync(m_jsonClient, call.dump().c_str(), s_buffer, sizeof(s_buffer), ClientAsyncReplyHandler);

				s_asyncSem.A();
				Assert::AreEqual(asyncId, s_asyncId);
			}

			Assert::AreEqual(i, uint16_t(RPC_CALL_ITERATIONS));
		}
		
		TEST_METHOD(AsyncCallSumTwoNumbersAndStop) {
			json call;
			call["function"] = "SumNumbers";
			json parameters = json::array();
			{
				json param;
				param["type"] = "INT16";
				param["value"] = 1234;
				parameters += param;
			}
			{
				json param;
				param["type"] = "INT16";
				param["value"] = 4321;
				parameters += param;
			}
			call["parameters"] = parameters;

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				AsyncID asyncId = RpcCallAsync(m_jsonClient, call.dump().c_str(), s_buffer, sizeof(s_buffer), ClientAsyncReplyHandler);

				s_asyncSem.A();
				Assert::AreEqual(unsigned long(5555), s_result);
				Assert::AreEqual(asyncId, s_asyncId);
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
		}

		TEST_METHOD(CallConcatTwoStringsAndStop) {
			json call;
			call["function"] = "Concatenate";
			json parameters = json::array();
			{
				json param;
				param["type"] = "STRING_REF";
				param["value"] = "foo";
				parameters += param;
			}
			{
				json param;
				param["type"] = "INT16";
				param["value"] = 2;
				parameters += param;
			}
			call["parameters"] = parameters;
			
			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				RpcCall(m_jsonClient, call.dump().c_str(), s_buffer, sizeof(s_buffer));
				json jresult = json::parse(s_buffer);
				string result = jresult["parameters"][0]["value"];
				Assert::AreEqual(string("foofoofoo"), result);
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
		}
	
		TEST_METHOD(AsyncCallConcatTwoStringsAndStop) {
			json call;
			call["function"] = "Concatenate";
			json parameters = json::array();
			{
				json param;
				param["type"] = "STRING_REF";
				param["value"] = "foo";
				parameters += param;
			}
			{
				json param;
				param["type"] = "INT16";
				param["value"] = 2;
				parameters += param;
			}
			call["parameters"] = parameters;

			int i = 0;
			while (i++ < RPC_CALL_ITERATIONS) {
				RpcCallAsync(m_jsonClient, call.dump().c_str(), s_buffer, sizeof(s_buffer), ClientAsyncReplyHandler);

				s_asyncSem.A();
				json jresult = json::parse(s_buffer);
				string result = jresult["parameters"][0]["value"];
				Assert::AreEqual(string("foofoofoo"), result);
			}

			Assert::AreEqual(i, RPC_CALL_ITERATIONS + 1);
		}
	};

	AsyncID			SingleJsonClientSingleJsonServerTests::s_asyncId;
	unsigned long   SingleJsonClientSingleJsonServerTests::s_result;
	Semaphore		SingleJsonClientSingleJsonServerTests::s_asyncSem(0);
	unsigned long	SingleJsonClientSingleJsonServerTests::s_asyncResult;
	char			SingleJsonClientSingleJsonServerTests::s_buffer[JSON_BUFFER_SIZE];

	uint64_t SingleJsonClientSingleJsonServerTests::SumNumbers(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
		json call = json::parse(jsonCallP);

		int16_t num1 = call["parameters"][1]["value"];
		int16_t num2 = call["parameters"][2]["value"];
		int16_t sum = num1 + num2;

		string jsonCallResult = call.dump();
		if (jsonCallResult.length() < jsonCallResultLen)
			memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

		return (unsigned long)sum;
	}

	uint64_t SingleJsonClientSingleJsonServerTests::Nop(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
		json call = json::parse(jsonCallP);

		string jsonCallResult = call.dump();
		if (jsonCallResult.length() < jsonCallResultLen)
			memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

		return (unsigned long)0;
	}

	uint64_t SingleJsonClientSingleJsonServerTests::Increment(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
		json call = json::parse(jsonCallP);

		int16_t value = call["parameters"][1]["value"];
		value++;
		call["parameters"][1]["value"] = value;

		string jsonCallResult = call.dump();
		if (jsonCallResult.length() < jsonCallResultLen)
			memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

		return (unsigned long)value;
	}

	uint64_t SingleJsonClientSingleJsonServerTests::Concatenate(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
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

	void SingleJsonClientSingleJsonServerTests::ClientAsyncReplyHandler(AsyncID asyncId, unsigned long result) {
		s_asyncId = asyncId;
		s_result = result;
		s_asyncSem.R();
	}
}
