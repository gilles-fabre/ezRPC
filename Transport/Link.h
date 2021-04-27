#ifndef _LINK_H
#define _LINK_H

#ifdef WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS  1
#endif

#ifdef WIN32
#include <ws2tcpip.h> 

#pragma comment( lib, "ws2_32.lib") // yuk!
#else
#include <unistd.h>
#include <sys/socket.h>
#endif

#include <iostream>

#include <fcntl.h>
#include <errno.h>
#include <log2reporter.h>

//#define LINK_TRACES 1
#define LINK_MODULE 0x2

#ifdef WIN32
#define MSG_DONTWAIT 0
#define MSG_NOSIGNAL 0
#else
#define SOCKET int
#endif

using namespace std;

/**
 * \class Link
 * \brief Simply send and receive data on the in/out link created
 *        by the parent transport.
 */
class Link {
	SOCKET m_in;	// in/out file descriptors (might be the same)
	SOCKET m_out;

public:
	Link(SOCKET in, SOCKET out) {
		m_in = in;
		m_out = out;
	}

	~Link() {
		Close();
	}

	bool Send(unsigned char *byte_buffer, int data_len);
	bool Send(unsigned char *byte_buffer, int *data_lenP);
	bool Receive(unsigned char *byte_buffer, int data_len);
	bool Receive(unsigned char *byte_buffer, int *data_lenP);
	bool Peek(unsigned char *byte_buffer, int *data_lenP);
	void WaitForRead() {
        fd_set 	        readset;
        int 			result;

        do {
           FD_ZERO(&readset);
           FD_SET(m_out, &readset);
           result = select((int)m_out + 1, &readset, NULL, NULL, NULL);
        } while (result == -1 && errno == EINTR);
	}
	void WaitForWrite() {
        fd_set 	        writeset;
        int 			result;

        do {
           FD_ZERO(&writeset);
           FD_SET(m_out, &writeset);
           result = select((int)m_out + 1, NULL, &writeset, NULL, NULL);
        } while (result == -1 && errno == EINTR);
	}
	void Close() {
#ifdef LINK_TRACES
		LogVText(LINK_MODULE, 0, true, "Link::Close(), in socket == %d, out socket == %d",  m_in, m_out);
#endif
		// close the in/out file descriptors
		if (m_in != -1 && m_in != m_out)
#ifdef WIN32
			closesocket(m_in);
#else
			close(m_in);
#endif
		if (m_out != -1)
#ifdef WIN32
			closesocket(m_out);
#else
			close(m_out);
#endif

		m_in = -1;
		m_out = -1;
	}
};

#endif // _LINK_H

