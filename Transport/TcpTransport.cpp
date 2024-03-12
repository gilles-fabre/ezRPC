#include <stdio.h>

#include <sys/types.h>  
#include <sys/stat.h>   
#include <fcntl.h>

#ifdef WIN32
#define UNIX_PATH_MAX 108
struct sockaddr_un {
	unsigned short		sun_family;
	char				sun_path[UNIX_PATH_MAX];  // pathname 
};

#else
#include <sys/socket.h>
#include <netinet/in.h> // PF_INET/AF_INET
#include <arpa/inet.h>
#include <sys/un.h>		// AF_UNIX
#endif

#include "TcpTransport.h"

using namespace std;

#ifdef WIN32
uint8_t TcpTransport::m_WSAStartupDone = 0;
#endif

/**
 * \fn Link* TcpTransport::WaitForLinkRequest(const string& serverAddress)
 * \brief Waits for a LinkRequest and returns the resulting link
 *
 * \param server_address is the IP address to listen on
 * 
 * \return (Link*)ReturnValue a valid ptr to a Link instance, NULL else (and the
 *			error code is set).
 */
ReturnValue<Link*, CommunicationErrors> TcpTransport::WaitForLinkRequest(const string& serverAddress) {
	ReturnValue<Link*, CommunicationErrors>	r;
	SOCKET									connSocket;
	struct 	sockaddr_in						server_addr = {0,};

#ifdef WIN32
	struct 	sockaddr 	server_storage;
	int					addr_size;
#else
	struct 	sockaddr_storage 	server_storage;
	socklen_t 					addr_size;
#endif
	string	ipAddr;
	string	port;
	int		portNum;
	int 	retval;
	int 	off = 0;

#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 0, true, "TcpTransport::WaitForLinkRequest(%s)", serverAddress.c_str());
#endif

	if (serverAddress.empty()) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: invalid server address!" << endl;
		r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::InvalidAddress };
		return r;
	}

	// get the server port
	size_t found = serverAddress.find_last_of(':');
	if (found ==  string::npos) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: server address misformated (ip_addr:port_number expected)!" << endl;
		r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::InvalidAddress };
		return r;
	}

	// check numeric port set
	port = serverAddress.substr(found + 1);
	istringstream ps(port);
	ps >> portNum;
	if (ps.fail()) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: non numeric address port!" << endl;
		r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::InvalidAddress };
		return r;
	}

	ipAddr = serverAddress.substr(0, found);
	if (ipAddr.empty())
		ipAddr = "0.0.0.0";
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "found host addr %s and port %d", ipAddr.c_str(), portNum);
#endif

	// create the server socket
	if (m_srvSocket == -1) {
		if ((m_srvSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't create socket (" << strerror(errno) << ")" << endl;
			r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::SocketCreationError };
			return r;
		}

		// configure settings of the server address struct
		// address family = Internet
		server_addr.sin_family = AF_INET;

		// set port number, using htons function to use proper byte order
		server_addr.sin_port = htons(portNum);

		// set IP address to localhost
		server_addr.sin_addr.s_addr = inet_addr(ipAddr.c_str());

		// bind the address struct to the socket
		bind(m_srvSocket, (struct sockaddr*)&server_addr, sizeof(server_addr));
#ifdef TRANSPORT_TRACES
		LogVText(TRANSPORT_MODULE, 4, true, "created and bound to socket %d", m_srvSocket);
#endif

		if (setsockopt(m_srvSocket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&off, sizeof(off)) < 0) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't set socket option (" << strerror(errno) << ")" << endl;
#ifdef WIN32
			closesocket(m_srvSocket);
#else
			close(m_srvSocket);
#endif    
			m_srvSocket = -1;
			r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::SocketSettingError };
			return r;
		}
	}
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "will now listen on socket %d", m_srvSocket);
#endif

	// listen on the socket, with 1 max connection request queued
	if ((retval = listen(m_srvSocket, 1))) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't listen on socket (" << strerror(errno) << ")" << endl;
		r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::SocketListeningError };
		return r;
	}
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "accepting connection on socket %d", m_srvSocket);
#endif

	// accept call creates a new socket for the incoming connection
	addr_size = sizeof(server_storage);
	if ((connSocket = accept(m_srvSocket, (struct sockaddr*)&server_storage, &addr_size)) == -1) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't accept connection (" << strerror(errno) << ")" << endl;
		r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::SocketConnectionError };
		return r;
	}

	if (setsockopt(connSocket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&off, sizeof(off)) < 0) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't set socket option (" << strerror(errno) << ")" << endl;
#ifdef WIN32
		closesocket(connSocket);
#else
		close(connSocket);
#endif
		connSocket = -1;
		r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::SocketSettingError };
		return r;
	}

	Link* newLinkP = new Link(connSocket, connSocket);
	m_links.push_back(newLinkP);
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "will return link %p", newLinkP);
#endif

	r = ReturnValue<Link*, CommunicationErrors>{ newLinkP, CommunicationErrors::ErrorCode::None };
	return r;
}

/**
 * \fn Link* TcpTransport::LinkRequest(const string& serverAddress)
 * \brief Connects to a server blocked on WaitLinkRequest and
 * 		  returns the resulting link
 *
 * \param server_address is the IP addr of the server to connect to
 * 
 * \return (Link*)ReturnValue a valid ptr to a Link instance, NULL else (and the
 *		   error code is set).
 */
ReturnValue<Link*, CommunicationErrors> TcpTransport::LinkRequest(const string& serverAddress) {
	ReturnValue<Link*, CommunicationErrors> r;
	SOCKET 								    connSocket;
	struct 	sockaddr_in					    server_addr = {0,};
	string								    ipAddr;
	string								    port;
	int									    portNum;
	int 								    retval;

#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 0, true, "TcpTransport::LinkRequest(%s)", serverAddress.c_str());
#endif

	if (serverAddress.empty()) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: empty address!" << endl;
		r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::InvalidAddress };
		return r;
	}

	// get the server port
	size_t found = serverAddress.find_last_of(':');
	if (found ==  string::npos) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: server address misformated (ip_addr:port_number expected)!" << endl;
		r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::InvalidAddress };
		return r;
	}

	// check numeric port set
	port = serverAddress.substr(found + 1);
	istringstream ps(port);
	ps >> portNum;
	if (ps.fail()) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: non numeric address port!" << endl;
		r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::InvalidAddress };
		return r;
	}

	ipAddr = serverAddress.substr(0, found);
	if (ipAddr.empty())
		ipAddr = "0.0.0.0";
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "found host addr %s and port %d", ipAddr.c_str(), portNum);
#endif

	// create the client socket
	if ((connSocket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't create socket (" << strerror(errno) << ")" << endl;
		r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::SocketCreationError };
		return r;
	}

	// configure settings of the server address struct
	// address family = Internet
	server_addr.sin_family = AF_INET;

	// set port number, using htons function to use proper byte order
	server_addr.sin_port = htons(portNum);

	// set IP address to localhost
	server_addr.sin_addr.s_addr = inet_addr(ipAddr.c_str());
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "created and will connect to socket %d", connSocket);
#endif

	// connect the socket to the server using the address struct
	if ((retval = connect(connSocket, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1)  {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't connect to socket (" << strerror(errno) << ")" << endl;
#ifdef WIN32
		closesocket(connSocket);
#else
		close(connSocket);
#endif    
		r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::SocketConnectionError };
		return r;
	}

	Link* newLinkP = new Link(connSocket, connSocket);
	m_links.push_back(newLinkP);
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "will return link %p", newLinkP);
#endif

	r = ReturnValue<Link*, CommunicationErrors>{ newLinkP, CommunicationErrors::ErrorCode::None };
	return r;
}
