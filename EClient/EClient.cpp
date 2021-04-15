#include <iostream>
#include <log2reporter.h>

#include "EClient.h"

// EClient callbacks....

/**
 * \fn void TransitionCallback(SMID state_machine_id, const std::string &from_state_name, const std::string &transition_name, const std::string &to_state_name, void *state_machine_user_data)
 * \brief this is the default and fallback (in case no remote procedure is defined in the
 *        json state machine definition file) callback. All remote procedures must respect
 *        this prototype.
 *
 * \param v is a pointer to a remote procedure parameters vector. Modified pointed values
 *        will be returned to the caller.
 *        The first parameter shall be ignored (it is the return value result address and is
 *        used by the rpc mechanism to automatically return the result of this procedure).
 *        Then come the uint64 SMID (state machine ID), the SM name (string), the source
 *        state name (string), the executed transition name (string), the destination
 *        state name (string) and the user data pointer (passed when the machine was created).
 *
 * \return -1 upon error, 0 else.
 */
unsigned long EClient::TransitionCallback(vector<RemoteProcedureCall::Parameter *> *v) {
#ifdef ECLIENT_TRACES
	LogVText(ECLIENT_MODULE, 0, true, "EClient::TransitionCallback(%p)", v);
#endif

	if (!v || v->size() != 7)
		return (unsigned long)-1;

#ifdef ECLIENT_TRACES
	//RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	RemoteProcedureCall::Parameter *p2 = (*v)[2];
	RemoteProcedureCall::Parameter *p3 = (*v)[3];
	RemoteProcedureCall::Parameter *p4 = (*v)[4];
	RemoteProcedureCall::Parameter *p5 = (*v)[5];
	RemoteProcedureCall::Parameter *p6 = (*v)[6];

	uint64_t sm_id = p1->GetUInt64Value();
	LogVText(ECLIENT_MODULE, 4, true, "machine id: %x", sm_id);
	string   &sm_name = p2->GetStringValue();
	LogVText(ECLIENT_MODULE, 4, true, "machine name: %s", sm_name.c_str());
	string   &src_state = p3->GetStringValue();
	LogVText(ECLIENT_MODULE, 4, true, "source state: %s", src_state.c_str());
	string   &transition = p4->GetStringValue();
	LogVText(ECLIENT_MODULE, 4, true, "transition: %s", transition.c_str());
	string &dst_state = p5->GetStringValue();
	LogVText(ECLIENT_MODULE, 4, true, "destination state: %s", dst_state.c_str());
	void *user_dataP = (void *)p6->GetUInt64Value();
	LogVText(ECLIENT_MODULE, 4, true, "user data: %p", user_dataP);
#endif

	return 0;
}

// CODEC CALLBACKS

// std::string *GetMessageFieldCallback(MID message_id, const std::string &field_name, void *codec_user_data, void *message_user_data);
unsigned long EClient::GetMessageFieldCallback(vector<RemoteProcedureCall::Parameter *> *v) {
	return (unsigned long)-1;
}

// void MessageReceivedCallback(MID message_id, const std::string &json_message_definition, void *codec_user_data, void *message_user_data);
unsigned long EClient::MessageReceivedCallback(vector<RemoteProcedureCall::Parameter *> *v) {
	return (unsigned long)-1;
}

// DATA EXCHANGE (with peer equipment) CALLBACK

// bool SendDataCallback(unsigned char *message_data, unsigned int message_data_len, void *codec_user_data, void *message_user_data);
unsigned long EClient::SendDataCallback(vector<RemoteProcedureCall::Parameter *> *v) {
	return (unsigned long)-1;
}

// unsigned char *ReceiveDataCallback(unsigned char *byte_buffer, unsigned int data_len, void *codec_user_data, void *message_user_data);
unsigned long EClient::ReceiveDataCallback(vector<RemoteProcedureCall::Parameter *> *v) {
	return (unsigned long)-1;
}

// STATE MACHINE APIs

unsigned long EClient::CreateStateMachine(const string &json_definition_file, void *user_dataP) {
	return m_epe_client.RpcCall("CreateStateMachine",
								RemoteProcedureCall::STRING,
								m_client_address,
								RemoteProcedureCall::STRING,
								json_definition_file,
								RemoteProcedureCall::UINT64,
								(uint64_t)user_dataP,
								RemoteProcedureCall::END_OF_CALL);
}

unsigned long EClient::DestroyStateMachine(SMID state_machine_id) {
	return m_epe_client.RpcCall("DestroyStateMachine",
								RemoteProcedureCall::STRING,
								m_client_address,
								RemoteProcedureCall::UINT64,
								(uint64_t)state_machine_id,
								RemoteProcedureCall::END_OF_CALL);
}

unsigned long EClient::DoTransition(SMID state_machine_id, const string &transition_name) {
	return m_epe_client.RpcCall("DoTransition",
								RemoteProcedureCall::STRING,
								m_client_address,
								RemoteProcedureCall::UINT64,
								(uint64_t)state_machine_id,
								RemoteProcedureCall::STRING,
								transition_name,
								RemoteProcedureCall::END_OF_CALL);
}

