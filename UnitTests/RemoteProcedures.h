#pragma once

#include <RemoteProcedureCall.h>

RpcReturnValue Nop(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
RpcReturnValue Increment(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
RpcReturnValue IncDouble(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
RpcReturnValue SumNumbers(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);
RpcReturnValue Concatenate(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP);

uint64_t JsonNop(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
uint64_t JsonIncrement(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
uint64_t JsonIncDouble(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
uint64_t JsonConcatenate(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
uint64_t JsonSumNumbers(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
