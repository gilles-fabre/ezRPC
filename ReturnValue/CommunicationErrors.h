#pragma once 

class CommunicationErrors {
public:
  typedef enum _ErrorCode { None, 
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
							END } ErrorCode;
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
											  "RpcParametersError" };
};



