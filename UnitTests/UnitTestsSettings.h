#pragma once

#define  RPC_CALL_ITERATIONS 5
#ifdef TCP_TESTS
#define  RPC_SERVER_ADDRESS  "127.0.0.1:4444"
#define  RPC_BAD_SERVER_ADDRESS  "127.0.0.1:4445"
#define  RPC_TRANSPORT		 Transport::TCP
#else
#define  RPC_SERVER_ADDRESS  "\\server_file.bin"
#define  RPC_BAD_SERVER_ADDRESS  "\\bad_server_file.bin"
#define  RPC_TRANSPORT		 Transport::FILE
#endif
#define JSON_BUFFER_SIZE	 1024
#define SERVER_COMM_SETUP_DELAY 250ms