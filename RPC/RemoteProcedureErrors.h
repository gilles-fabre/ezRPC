#pragma once


class RemoteProcedureErrors {
public:
	typedef enum _ErrorCode {
		None,
		EmptyResult,
		BadArgument,
		AllocationError,
		MissingData,
		CommunicationDropped,
		BadProtocol,
		SocketCreationError,
		SocketConnectionError,
		InvalidAddress,
		SocketSettingError,
		SocketListeningError,
		ProtocolError,
		RpcParametersError,
		WrongNumberOfArguments,
		NullPointer,
		JsonParsingException,
		JsonResultBufferTooSmall,
		END
	} ErrorCode;
	static constexpr const char* m_errors[] = { "None",
												"EmptyResult",
												"BadArgument",
												"AllocationError",
												"MissingData",
												"CommunicationDropped",
												"BadProtocol",
												"SocketCreationError",
												"SocketConnectionError",
												"InvalidAddress",
												"SocketSettingError",
												"SocketListeningError",
												"ProtocolError",
												"RpcParametersError",
												"WrongNumberOfArguments",
												"NullPointer",
												"JsonParsingException", 
												"JsonResultBufferTooSmall"};
};


