#include <RemoteProcedureCall.h>

class JsonParameters {
public:
	static bool BuildParametersFromJson(const char* jsonCallP, string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params);
	static bool BuildJsonFromParameters(string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params, string& jsonResult);
	static void CleanupParameters(shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params);
};
