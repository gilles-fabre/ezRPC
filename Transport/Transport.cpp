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
#include "TcpTransport.h"
#include "FileTransport.h"

using namespace std;

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

