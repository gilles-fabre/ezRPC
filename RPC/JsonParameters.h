#include <RemoteProcedureCall.h>

#ifdef _DEBUG
#define JSONPARAMETERS_TRACES 1
#endif

#define JSONPARAMETERS_MODULE 0x30

class JsonParameters {
public:
	static bool BuildParametersFromJson(const char* jsonCallP, string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params);
	static bool BuildJsonFromResultParameters(const string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params, string& jsonResult);
	static bool BuildJsonFromCallParameters(const string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params, string& jsonResult);
	static void CleanupParameters(shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params);
};
