#include "pch.h"
#include "CppUnitTest.h"

#include <RpcServer.h>
#include <RpcClient.h>
#include <Transport.h>

#include <thread>

#include <JsonRPCServer.h>
#include <JsonRPCClient.h>

#include <nlohmann/json.hpp>

#include "UnitTestsSettings.h"
#include "RemoteProcedures.h"

using json = nlohmann::json;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FILE_RPCtests {

	TEST_CLASS(SingleJsonClientSingleJsonServerTests) {
	public:
		static AsyncID			s_asyncId;
		static uint64_t			s_result;
		static Semaphore		s_asyncSem;
		static uint64_t			s_asyncResult;
		static char				s_buffer[JSON_BUFFER_SIZE];
		uint64_t				m_jsonServer;
		uint64_t				m_jsonClient;

		static void		ClientAsyncReplyHandler(AsyncID asyncId, uint64_t result);

		SingleJsonClientSingleJsonServerTests() {
			m_jsonServer = CreateRpcServer(RPC_TRANSPORT, RPC_SERVER_ADDRESS);

			RegisterProcedure(m_jsonServer, "Nop", &JsonNop);
			RegisterProcedure(m_jsonServer, "SumNumbers", &JsonSumNumbers);
			RegisterProcedure(m_jsonServer, "Increment", &JsonIncrement);
			RegisterProcedure(m_jsonServer, "Concatenate", &JsonConcatenate);

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

		TEST_METHOD(MissingArgumentsReturnAnError) {
			json call;
			call["function"] = "SumNumbers";
			json parameters = json::array();
			{
				json param;
				param["type"] = "INT16";
				param["value"] = 1234;
				parameters += param;
			}
			call["parameters"] = parameters;

			RpcReturnValue result = RpcCall(m_jsonClient, call.dump().c_str(), s_buffer, sizeof(s_buffer));
			Assert::IsTrue(result.IsError());
			Assert::AreEqual(result.GetErrorString(), string("WrongNumberOfArguments"));
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
				uint64_t result = RpcCall(m_jsonClient, call.dump().c_str(), s_buffer, sizeof(s_buffer));
				Assert::AreEqual(uint64_t(5555), result);
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
				Assert::AreEqual(uint64_t(5555), s_result);
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
	uint64_t		SingleJsonClientSingleJsonServerTests::s_result;
	Semaphore		SingleJsonClientSingleJsonServerTests::s_asyncSem(0);
	uint64_t		SingleJsonClientSingleJsonServerTests::s_asyncResult;
	char			SingleJsonClientSingleJsonServerTests::s_buffer[JSON_BUFFER_SIZE];

	void SingleJsonClientSingleJsonServerTests::ClientAsyncReplyHandler(AsyncID asyncId, uint64_t result) {
		s_asyncId = asyncId;
		s_result = result;
		s_asyncSem.R();
	}
}
