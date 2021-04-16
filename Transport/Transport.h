#ifndef _TRANSPORT_H
#define _TRANSPORT_H

#ifdef WIN32
#pragma warning(disable:4996)
#endif

#include <string.h>

#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <log2reporter.h>

#include "Link.h"

using namespace std;

#ifdef WIN32
#define SO_REUSEPORT 0
#endif

#define TRANSPORT_TRACES 1
#define TRANSPORT_MODULE 0x2

/**
* \class Transport
* \brief Abstract transport over any of the supported transports.
*/
class Transport {
protected:
	vector<Link *> m_links; // the communication links

public:
	enum TransportType{TCP,
				       FILE};

	Transport() {
#ifdef TRANSPORT_TRACES
		LogVText(TRANSPORT_MODULE, 0, true, "Transport::Transport(), creating transport %p", this);
#endif
	}

	virtual ~Transport() {
#ifdef TRANSPORT_TRACES
		LogVText(TRANSPORT_MODULE, 0, true, "Transport::~Transport(), deleting transport %p", this);
#endif
	}

	static Transport *CreateTransport(TransportType transport_type);

	virtual Link* LinkRequest(const string &server_address) = 0;
	virtual Link* WaitForLinkRequest(const string &server_address) = 0;

	virtual void Close() {
#ifdef TRANSPORT_TRACES
		LogVText(TRANSPORT_MODULE, 0, true, "Transport::Close(), closing transport %p", this);
#endif
		// close all links
		for (vector<Link *>::iterator it = m_links.begin(); it != m_links.end(); it++)
			delete (*it);
		m_links.clear();
	}

	/**
	 * \brief Release the link so the caller can delete it
	 */
	void ReleaseLink(Link *linkP) {
		for (vector<Link *>::iterator i = m_links.begin(); i != m_links.end(); i++) {
			if (*i == linkP) {
				m_links.erase(i);
				return;
			}
		}
	}
};

class TcpTransport : public Transport {
	SOCKET m_s_socket;

public:
	TcpTransport() : Transport() {m_s_socket = -1;}
	~TcpTransport() {
		Close();
	}

	Link* LinkRequest(const string &server_address);
	Link* WaitForLinkRequest(const string &server_address);
	void Close() {
		Transport::Close();

#ifdef TRANSPORT_TRACES
		LogVText(TRANSPORT_MODULE, 0, true, "TcpTransport::Close(), closing listening socket %d", m_s_socket);
#endif
		// don't need the server socket anymore
		if (m_s_socket != -1)
#ifdef WIN32
			closesocket(m_s_socket);
#else
			close(m_s_socket);
#endif
		m_s_socket = -1;
	}
};

#ifndef WIN32
class FileTransport : public Transport {
	int 	m_s_socket;
	string	m_server_address;

public:
	FileTransport() : Transport() {m_s_socket = -1;}
	~FileTransport() {
		Close();
	}

	Link* LinkRequest(const string &server_address);
	Link* WaitForLinkRequest(const string &server_address);
	void  Close() {
		Transport::Close();

#ifdef TRANSPORT_TRACES
		LogVText(TRANSPORT_MODULE, 0, true, "FileTransport::Close(), closing listening socket %d", m_s_socket);
#endif
		// don't need the server socket anymore
		if (m_s_socket != -1)
			close(m_s_socket);

		// don't need the file anymore
		if (!m_server_address.empty()) {
#ifdef TRANSPORT_TRACES
			LogVText(TRANSPORT_MODULE, 0, true, "TcpTransport::Close(), deleting file %s", m_server_address.c_str());
#endif
			unlink(m_server_address.c_str()); // this file should already have been deleted...
		}
	}
};
#endif // ifndef WIN32 -- no AF_UNIX support under windows

#endif // _TRANSPORT_H

