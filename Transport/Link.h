#ifndef _LINK_H
#define _LINK_H

#include <iostream>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <log2reporter.h>

#define LINK_TRACES 1
#define LINK_MODULE 0x1

using namespace std;

/**
 * \class Link
 * \brief Simply send and receive data on the in/out link created
 *        by the parent transport.
 */
class Link {
	int m_in;	// in/out file descriptors (might be the same)
	int m_out;

public:
	Link(int in, int out) {
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
           result = select(m_out + 1, &readset, NULL, NULL, NULL);
        } while (result == -1 && errno == EINTR);
	}
	void WaitForWrite() {
        fd_set 	        writeset;
        int 			result;

        do {
           FD_ZERO(&writeset);
           FD_SET(m_out, &writeset);
           result = select(m_out + 1, NULL, &writeset, NULL, NULL);
        } while (result == -1 && errno == EINTR);
	}
	void Close() {
#ifdef LINK_TRACES
		LogVText(LINK_MODULE, 0, true, "Link::Close(), in socket == %d, out socket == %d",  m_in, m_out);
#endif
		// close the in/out file descriptors
		if (m_in != -1 && m_in != m_out)
			close(m_in);

		if (m_out != -1)
			close(m_out);

		m_in = -1;
		m_out = -1;
	}
	bool IsConnected() {
		char c;
		return send(m_out, &c, 1, MSG_DONTWAIT | MSG_NOSIGNAL) != -1;
	}
};

#endif // _LINK_H

