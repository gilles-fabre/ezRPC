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

#include "FileTransport.h"

using namespace std;

/**
 * \fn Link* FileTransport::WaitForLinkRequest(const string& server_address)
 * \brief Waits for a LinkRequest and returns the resulting link
 *
 * \param server_address is the file name to listen on
 * 
 * \return (Link*)ReturnValue a valid ptr to a Link instance, NULL else (and the
 *		   error code is set).
 */
ReturnValue<Link*, CommunicationErrors> FileTransport::WaitForLinkRequest(const string& server_address) {
	ReturnValue<Link*, CommunicationErrors>	r;
	int 									connSocket;
	sockaddr_un 							server_addr = { 0, };
	struct sockaddr_storage					server_storage;
	socklen_t 								addr_size;
	int 									retval;
	int 									off = 0;

#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 0, true, "FileTransport::WaitForLinkRequest(%s)", server_address.c_str());
#endif

	if (server_address.empty()) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: empty filename!" << endl;
		r = ReturnValue<Link*, CommunicationErrors>{NULL, CommunicationErrors::ErrorCode::InvalidAddress};
		return r;
	}

	if (m_srvSocket == -1) {
		// keep the file name to later unlink
		m_serverAddress = server_address;

		// in case the file already exist, must delete it
		unlink(m_serverAddress.c_str());

		// create the server socket
		if ((m_srvSocket = (int)socket(AF_UNIX, SOCK_STREAM, IPPROTO_TCP)) == -1) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't create socket (" << strerror(errno) << ")" << endl;
			r = ReturnValue<Link*, CommunicationErrors>{ NULL, CommunicationErrors::ErrorCode::SocketCreationError };
			return r;
		}

		// configure settings of the server address struct
		// address family = Internet
		server_addr.sun_family = AF_UNIX;

		// set IP address to localhost
		strcpy(server_addr.sun_path, server_address.c_str());

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
			close(m_s_socket);
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
	LogVText(TRANSPORT_MODULE, 4, true, "accepting connection on socket", m_srvSocket);
#endif

	// accept call creates a new socket for the incoming connection
	addr_size = sizeof(server_storage);
	connSocket = (int)accept(m_srvSocket, (struct sockaddr*)&server_storage, &addr_size);

	if (setsockopt(connSocket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&off, sizeof(off)) < 0) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't set socket option (" << strerror(errno) << ")" << endl;
#ifdef WIN32
		closesocket(connSocket);
#else
		close(c_socket);
#endif
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
 * \fn Link* FileTransport::LinkRequest(const string& server_address)
 * \brief Connects to a server blocked on WaitLinkRequest and
 * 		  returns the resulting link
 *
 * \param server_address is the filename the server is listening on
 * 
 * \return (Link*)ReturnValue a valid ptr to a Link instance, NULL else (and the
 *		   error code is set).
 */
ReturnValue<Link*, CommunicationErrors> FileTransport::LinkRequest(const string& server_address) {
	ReturnValue<Link*, CommunicationErrors>	r;
	int 									connSocket;
	sockaddr_un								server_addr = { 0, };
	int 									retval;
	
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 0, true, "FileTransport::LinkRequest(%s)", server_address.c_str());
#endif

	// create the client socket
	if ((connSocket = (int)socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't create socket (" << strerror(errno) << ")" << endl;
		r = ReturnValue<Link*, CommunicationErrors>{NULL, CommunicationErrors::ErrorCode::SocketCreationError};
		return r;
	}

	// configure settings of the server address struct
	// address family = Internet
	server_addr.sun_family = AF_UNIX;

	// set IP address to localhost
    strcpy(server_addr.sun_path, server_address.c_str());
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "created and will connect to socket %d", connSocket);
#endif

	// connect the socket to the server using the address struct
	if ((retval = connect(connSocket, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1)  {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't connect to socket (" << strerror(errno) << ")" << endl;
#ifdef WIN32
		closesocket(connSocket);
#else
		close(c_socket);
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
