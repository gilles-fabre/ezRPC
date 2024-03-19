#pragma once

#include <RemoteProcedureCall.h>

RpcReturnValue Nop(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
RpcReturnValue Increment(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
RpcReturnValue IncDouble(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
RpcReturnValue SumNumbers(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
RpcReturnValue Concatenate(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
RpcReturnValue CancelProcedure(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
RpcReturnValue CancellableProcedure(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);

uint64_t JsonNop(AsyncID asyncId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
uint64_t JsonIncrement(AsyncID asyncId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
uint64_t JsonIncDouble(AsyncID asyncId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
uint64_t JsonConcatenate(AsyncID asyncId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
uint64_t JsonSumNumbers(AsyncID asyncId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
