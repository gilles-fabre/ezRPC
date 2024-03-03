#ifndef _TCP_TRANSPORT_H
#define _TCP_TRANSPORT_H

#include "Transport.h"

using namespace std;

class TcpTransport : public Transport {
#ifdef WIN32
	static uint8_t	m_WSAStartupDone; // more windows crap
#endif

	SOCKET			m_srvSocket;

public:
	TcpTransport() : Transport() {
#ifdef WIN32
		if (!m_WSAStartupDone) {
			WSADATA wsa;
			if (!WSAStartup(MAKEWORD(2, 2), &wsa))
				m_WSAStartupDone = -1;
		}
#endif

		m_srvSocket = -1;
	}
	
	~TcpTransport() {
		Close();
	}

	ReturnValue<Link*, CommunicationErrors>&& LinkRequest(const string &serverAddress);
	ReturnValue<Link*, CommunicationErrors>&& WaitForLinkRequest(const string &serverAddress);
	void Close() {
		Transport::Close();

#ifdef TRANSPORT_TRACES
		LogVText(TRANSPORT_MODULE, 0, true, "TcpTransport::Close(), closing listening socket %d", m_srvSocket);
#endif
		// don't need the server socket anymore
		if (m_srvSocket != -1)
#ifdef WIN32
			closesocket(m_srvSocket);
#else
			close(m_s_socket);
#endif
		m_srvSocket = -1;
	}
};

#endif // _TCP_TRANSPORT_H

