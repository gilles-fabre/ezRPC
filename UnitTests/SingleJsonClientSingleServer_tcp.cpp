#include "pch.h"
#include "CppUnitTest.h"

#include <RpcServer.h>
#include <RpcClient.h>
#include <Transport.h>

#include <thread>

#include <JsonRPCClient.h>

#include <nlohmann/json.hpp>

#define TCP
#include "UnitTestsSettings.h"

using json = nlohmann::json;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TCP_RPCtests {

	TEST_CLASS(SingleJsonClientSingleServerTests) {
	public:
		static AsyncID			s_asyncId;
		static unsigned long    s_result;
		static Semaphore		s_asyncSem;
		static unsigned long	s_asyncResult;
		static char				s_buffer[JSON_BUFFER_SIZE];

		RPCServer				m_rpcServer;
		uint64_t				m_jsonClient;

		static unsigned long Increment(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
		static unsigned long Nop(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
		static unsigned long SumNumbers(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
		static unsigned long Concatenate(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);

		static void		ClientAsyncReplyHandler(AsyncID asyncId, unsigned long result);

		SingleJsonClientSingleServerTests() : m_rpcServer(RPC_TRANSPORT, RPC_SERVER_ADDRESS) {
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

			m_jsonClient = CreateRpcClient(RPC_TRANSPORT, RPC_SERVER_ADDRESS);
		}

		~SingleJsonClientSingleServerTests() {
			m_rpcServer.Stop();
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

	AsyncID			SingleJsonClientSingleServerTests::s_asyncId;
	unsigned long   SingleJsonClientSingleServerTests::s_result;
	Semaphore		SingleJsonClientSingleServerTests::s_asyncSem(0);
	unsigned long	SingleJsonClientSingleServerTests::s_asyncResult;
	char			SingleJsonClientSingleServerTests::s_buffer[JSON_BUFFER_SIZE];

	unsigned long SingleJsonClientSingleServerTests::Increment(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
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

	unsigned long SingleJsonClientSingleServerTests::Nop(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
		return 0;
	}

	unsigned long SingleJsonClientSingleServerTests::SumNumbers(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
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

	unsigned long SingleJsonClientSingleServerTests::Concatenate(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
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

	void SingleJsonClientSingleServerTests::ClientAsyncReplyHandler(AsyncID asyncId, unsigned long result) {
		s_asyncId = asyncId;
		s_result = result;
		s_asyncSem.R();
	}
}
