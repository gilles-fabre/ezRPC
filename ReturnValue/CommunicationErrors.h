#pragma once 

class CommunicationErrors {
public:
  typedef enum _ErrorCode { None, 
							EmptyResult, 
							BadArgument, 
							MissingData, 
							CommunicationDropped, 
							BadProtocol, 
							SocketCreationError, 
							SocketConnectionError, 
							InvalidAddress, 
							SocketSettingError,
							SocketListeningError,
							END } ErrorCode;
  static constexpr const char* m_errors[] = { "None", 
											  "EmptyResult", 
											  "BadArgument", 
											  "MissingData", 
											  "CommunicationDropped", 
                                              "BadProtocol", 
											  "SocketCreationError", 
											  "SocketConnectionError", 
											  "InvalidAddress",
											  "SocketSettingError",
											  "SocketListeningError" };
};



