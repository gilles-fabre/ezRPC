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

#ifdef _DEBUG
#define TRANSPORT_TRACES 1
#endif
#define TRANSPORT_MODULE 0x3

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

	static ReturnValue<Transport*, CommunicationErrors>&& CreateTransport(TransportType transportType);

	virtual ReturnValue<Link*, CommunicationErrors>&& LinkRequest(const string &serverAddress) = 0;
	virtual ReturnValue<Link*, CommunicationErrors>&& WaitForLinkRequest(const string &serverAddress) = 0;

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

#endif // _TRANSPORT_H

