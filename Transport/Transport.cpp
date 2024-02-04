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

#include "Transport.h"

using namespace std;

#ifdef WIN32
uint8_t TcpTransport::m_WSAStartupDone = 0;
#endif

/**
 * \fn Transport* Transport::CreateTransport(TransportType transport_type)
 * \brief Returns a newly created transport for the given transport type
 *
 * \param transport_type specifies the requested type of underlying transport
 * \return an instance of specialized transport, NULL if an error occured
 */
Transport* Transport::CreateTransport(TransportType transport_type) {
	Transport* transport = NULL;
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 0, true, "Transport::CreateTransport(%d)", transport_type);
#endif

	switch (transport_type) {
		case TCP:
			transport = new TcpTransport();
#ifdef TRANSPORT_TRACES
			LogVText(TRANSPORT_MODULE, 4, true, "created transport %p of type TCP", transport);
#endif
			break;

		case FILE:
			transport = new FileTransport();
#ifdef TRANSPORT_TRACES
			LogVText(TRANSPORT_MODULE, 4, true, "created transport %p of type FILE", transport);
#endif
			break;

		default:
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: unknown Transport type!" << endl;
			break;
	}

	return transport;
}

/**
 * \fn Link* TcpTransport::WaitForLinkRequest(const string& server_address)
 * \brief Waits for a LinkRequest and returns the resulting link
 *
 * \param server_address is the IP address to listen on
 * \return a connected link to the peer, NULL upon error
 */
Link* TcpTransport::WaitForLinkRequest(const string& server_address) {
	SOCKET				c_socket;
	struct 	sockaddr_in server_addr = {0,};

#ifdef WIN32
	struct 	sockaddr 	server_storage;
	int					addr_size;
#else
	struct 	sockaddr_storage 	server_storage;
	socklen_t 					addr_size;
#endif
	string	s_ipaddr;
	string	s_port;
	int		n_port;
	int 	retval;
	int 	off = 0;

#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 0, true, "TcpTransport::WaitForLinkRequest(%s)", server_address.c_str());
#endif

	if (server_address.empty()) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: invalid server address!" << endl;
		return NULL;
	}

	// get the server port
	size_t found = server_address.find_last_of(':');
	if (found ==  string::npos) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: server address misformated (ip_addr:port_number expected)!" << endl;
		return NULL;
	}

	// check numeric port set
	s_port = server_address.substr(found + 1);
	istringstream ps(s_port);
	ps >> n_port;
	if (ps.fail()) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: non numeric address port!" << endl;
		return NULL; // port must be numeric
	}

	s_ipaddr = server_address.substr(0, found);
	if (s_ipaddr.empty())
		s_ipaddr = "0.0.0.0";
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "found host addr %s and port %d", s_ipaddr.c_str(), n_port);
#endif

	// create the server socket
	if (m_s_socket == -1) {
		if ((m_s_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't create socket (" << strerror(errno) << ")" << endl;
			return NULL;
		}

		// configure settings of the server address struct
		// address family = Internet
		server_addr.sin_family = AF_INET;

		// set port number, using htons function to use proper byte order
		server_addr.sin_port = htons(n_port);

		// set IP address to localhost
		server_addr.sin_addr.s_addr = inet_addr(s_ipaddr.c_str());

		// bind the address struct to the socket
		bind(m_s_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
#ifdef TRANSPORT_TRACES
		LogVText(TRANSPORT_MODULE, 4, true, "created and bound to socket %d", m_s_socket);
#endif

		if (setsockopt(m_s_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&off, sizeof(off)) < 0) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't set socket option (" << strerror(errno) << ")" << endl;
#ifdef WIN32
			closesocket(m_s_socket);
#else
			close(m_s_socket);
#endif    
			m_s_socket = -1;
			return NULL;
		}
	}
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "will now listen on socket %d", m_s_socket);
#endif

	// listen on the socket, with 1 max connection request queued
	if ((retval = listen(m_s_socket, 1))) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't listen on socket (" << strerror(errno) << ")" << endl;
		return NULL;
	}
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "accepting connection on socket %d", m_s_socket);
#endif

	// accept call creates a new socket for the incoming connection
	addr_size = sizeof(server_storage);
	if ((c_socket = accept(m_s_socket, (struct sockaddr*)&server_storage, &addr_size)) == -1) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't accept connection (" << strerror(errno) << ")" << endl;
		return NULL;
	}

	if (setsockopt(c_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&off, sizeof(off)) < 0) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't set socket option (" << strerror(errno) << ")" << endl;
#ifdef WIN32
		closesocket(c_socket);
#else
		close(c_socket);
#endif
		c_socket = -1;
		return NULL;
	}

	Link* new_linkP = new Link(c_socket, c_socket);
	m_links.push_back(new_linkP);
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "will return link %p", new_linkP);
#endif

	return new_linkP;
}

/**
 * \fn Link* TcpTransport::LinkRequest(const string& server_address)
 * \brief Connects to a server blocked on WaitLinkRequest and
 * 		  returns the resulting link
 *
 * \param server_address is the IP addr of the server to connect to
 * \return a connected link to the peer, NULL upon error
 */
Link* TcpTransport::LinkRequest(const string& server_address) {
	SOCKET 				c_socket;
	struct 	sockaddr_in server_addr = {0,};
	string				s_ipaddr;
	string				s_port;
	int					n_port;
	int 				retval;

#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 0, true, "TcpTransport::LinkRequest(%s)", server_address.c_str());
#endif

	if (server_address.empty()) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: empty address!" << endl;
		return NULL;
	}

	// get the server port
	size_t found = server_address.find_last_of(':');
	if (found ==  string::npos) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: server address misformated (ip_addr:port_number expected)!" << endl;
		return NULL;
	}

	// check numeric port set
	s_port = server_address.substr(found + 1);
	istringstream ps(s_port);
	ps >> n_port;
	if (ps.fail()) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: non numeric address port!" << endl;
		return NULL; // port must be numeric
	}

	s_ipaddr = server_address.substr(0, found);
	if (s_ipaddr.empty())
		s_ipaddr = "0.0.0.0";
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "found host addr %s and port %d", s_ipaddr.c_str(), n_port);
#endif

	// create the client socket
	if ((c_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't create socket (" << strerror(errno) << ")" << endl;
		return NULL;
	}

	// configure settings of the server address struct
	// address family = Internet
	server_addr.sin_family = AF_INET;

	// set port number, using htons function to use proper byte order
	server_addr.sin_port = htons(n_port);

	// set IP address to localhost
	server_addr.sin_addr.s_addr = inet_addr(s_ipaddr.c_str());
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "created and will connect to socket %d", c_socket);
#endif

	// connect the socket to the server using the address struct
	if ((retval = connect(c_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1)  {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't connect to socket (" << strerror(errno) << ")" << endl;
#ifdef WIN32
		closesocket(c_socket);
#else
		close(c_socket);
#endif    
		return NULL;
	}

	Link* new_linkP = new Link(c_socket, c_socket);
	m_links.push_back(new_linkP);
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "will return link %p", new_linkP);
#endif

	return new_linkP;
}

/**
 * \fn Link* FileTransport::WaitForLinkRequest(const string& server_address)
 * \brief Waits for a LinkRequest and returns the resulting link
 *
 * \param server_address is the file name to listen on
 * \return a connected link to the peer, NULL upon error
 */
Link* FileTransport::WaitForLinkRequest(const string& server_address) {
	int 					c_socket;
	sockaddr_un 			server_addr = { 0, };
	struct sockaddr_storage server_storage;
	socklen_t 				addr_size;
	int 					retval;
	int 					off = 0;

#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 0, true, "FileTransport::WaitForLinkRequest(%s)", server_address.c_str());
#endif

	if (server_address.empty()) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: empty filename!" << endl;
		return NULL;
	}

	if (m_s_socket == -1) {
		// keep the file name to later unlink
		m_server_address = server_address;

		// in case the file already exist, must delete it
		unlink(m_server_address.c_str());

		// create the server socket
		if ((m_s_socket = (int)socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't create socket (" << strerror(errno) << ")" << endl;
			return NULL;
		}

		// configure settings of the server address struct
		// address family = Internet
		server_addr.sun_family = AF_UNIX;

		// set IP address to localhost
		strcpy(server_addr.sun_path, server_address.c_str());

		// bind the address struct to the socket
		bind(m_s_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
#ifdef TRANSPORT_TRACES
		LogVText(TRANSPORT_MODULE, 4, true, "created and bound to socket %d", m_s_socket);
#endif

		if (setsockopt(m_s_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&off, sizeof(off)) < 0) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't set socket option (" << strerror(errno) << ")" << endl;
#ifdef WIN32
			closesocket(m_s_socket);
#else
			close(m_s_socket);
#endif
			m_s_socket = -1;
			return NULL;
		}
	}
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "will now listen on socket %d", m_s_socket);
#endif

	// listen on the socket, with 1 max connection request queued
	if ((retval = listen(m_s_socket, 1))) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't listen on socket (" << strerror(errno) << ")" << endl;
		return NULL;
	}
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "accepting connection on socket", m_s_socket);
#endif

	// accept call creates a new socket for the incoming connection
	addr_size = sizeof(server_storage);
	c_socket = (int)accept(m_s_socket, (struct sockaddr*)&server_storage, &addr_size);

	if (setsockopt(c_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&off, sizeof(off)) < 0) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't set socket option (" << strerror(errno) << ")" << endl;
#ifdef WIN32
		closesocket(c_socket);
#else
		close(c_socket);
#endif
		return NULL;
	}

	Link* new_linkP = new Link(c_socket, c_socket);
	m_links.push_back(new_linkP);
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "will return link %p", new_linkP);
#endif

	return new_linkP;
}

/**
 * \fn Link* FileTransport::LinkRequest(const string& server_address)
 * \brief Connects to a server blocked on WaitLinkRequest and
 * 		  returns the resulting link
 *
 * \param server_address is the filename the server is listening on
 * \return a connected link to the peer, NULL upon error
 */
Link* FileTransport::LinkRequest(const string& server_address) {
	int 		c_socket;
	sockaddr_un server_addr = { 0, };
	int 		retval;
	
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 0, true, "FileTransport::LinkRequest(%s)", server_address.c_str());
#endif

	// create the client socket
	if ((c_socket = (int)socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't create socket (" << strerror(errno) << ")" << endl;
		return NULL;
	}

	// configure settings of the server address struct
	// address family = Internet
	server_addr.sun_family = AF_UNIX;

	// set IP address to localhost
    strcpy(server_addr.sun_path, server_address.c_str());
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "created and will connect to socket %d", c_socket);
#endif

	// connect the socket to the server using the address struct
	if ((retval = connect(c_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1)  {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't connect to socket (" << strerror(errno) << ")" << endl;
#ifdef WIN32
		closesocket(c_socket);
#else
		close(c_socket);
#endif
		return NULL;
	}

	Link* new_linkP = new Link(c_socket, c_socket);
	m_links.push_back(new_linkP);
#ifdef TRANSPORT_TRACES
	LogVText(TRANSPORT_MODULE, 4, true, "will return link %p", new_linkP);
#endif

	return new_linkP;
}
