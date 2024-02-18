#ifndef _FILE_TRANSPORT_H
#define _FILE_TRANSPORT_H

#include "Transport.h"

using namespace std;

class FileTransport : public Transport {
	int 	m_srvSocket;
	string	m_serverAddress;

public:
	FileTransport() : Transport() {m_srvSocket = -1;}
	~FileTransport() {
		Close();
	}

	Link* LinkRequest(const string &serverAddress);
	Link* WaitForLinkRequest(const string &serverAddress);
	void  Close() {
		Transport::Close();

#ifdef TRANSPORT_TRACES
		LogVText(TRANSPORT_MODULE, 0, true, "FileTransport::Close(), closing listening socket %d", m_srvSocket);
#endif
		// don't need the server socket anymore
		if (m_srvSocket != -1)
#ifdef WIN32
			closesocket(m_srvSocket);
#else
			close(m_s_socket);
#endif

		// don't need the file anymore
		if (!m_serverAddress.empty()) {
#ifdef TRANSPORT_TRACES
			LogVText(TRANSPORT_MODULE, 0, true, "TcpTransport::Close(), deleting file %s", m_serverAddress.c_str());
#endif
			unlink(m_serverAddress.c_str()); // this file should already have been deleted...
		}
	}
};

#endif // _FILE_TRANSPORT_H

