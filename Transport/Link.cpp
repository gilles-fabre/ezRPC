#include "Link.h"

/**
 * \fn bool Link::Send(unsigned char *byte_buffer, unsigned int data_len)
 * \brief Sends data_len bytes from byte_buffer to the linked peer.
 *        This a blocking complete send.
 *
 *	\param byte_buffer points to the data to be sent
 *	\param data_len is the amount of data to be sent
 *	\return TRUE if all data could be sent, else returns FALSE.
 */
bool Link::Send(unsigned char *byte_buffer, int data_len) {
	int total = 0,
	    last;

	if (!byte_buffer || m_out == -1)
		return false;
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 0, true, "Link::Send(%p, %d)", byte_buffer, data_len);
#endif

	while (total != data_len) {
		if ((last = send(m_out, (const char *)&byte_buffer[total], data_len - total, MSG_NOSIGNAL)) == -1)
			return false;

		if (last == 0)
			return true;
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 8, true, "sent %d bytes", last);
#endif

		total += last;
	}
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 8, true, "sent %d total bytes", total);
#endif

	return true;
}

/**
 * \fn bool Link::Send(unsigned char *byte_buffer, unsigned int *data_lenP)
 * \brief Sends up to data_len bytes from byte_buffer to the linked peer.
 *        This a blocking potentially non complete send.
 *
 *	\param byte_buffer points to the data to be sent
 *	\param data_len is the max amount of data to be sent
 *	\return TRUE if data could be sent, else returns FALSE.
 */
bool Link::Send(unsigned char *byte_buffer, int *data_lenP) {
	if (!byte_buffer || !data_lenP || m_out == -1)
		return false;
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 0, true, "Link::Send(%p, %d)", byte_buffer, *data_lenP);
#endif

	*data_lenP = send(m_out, (const char *)byte_buffer, *data_lenP, MSG_NOSIGNAL);
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 8, true, "sent %d total bytes", *data_lenP);
#endif

	return *data_lenP > 0;
}

/**
 * \fn bool Link::Receive(unsigned char *byte_buffer, unsigned int data_len)
 * \brief Receives data_len bytes into byte_buffer from the linked peer.
 *        This a blocking complete receive.
 *
 *	\param byte_buffer points to the reception buffer
 *	\param data_len is the amount of data to be received
 *	\return TRUE if all data could be received, else returns FALSE.
 */
bool Link::Receive(unsigned char *byte_buffer, int data_len) {
	int total = 0,
		last;

	if (!byte_buffer || m_in == -1)
		return false;
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 0, true, "Link::Receive(%p, %d)", byte_buffer, data_len);
#endif

	while (total != data_len) {
		if ((last = recv(m_in, (char *)&byte_buffer[total], data_len - total, MSG_NOSIGNAL)) == -1)
			return false;

		if (last == 0)
			return false; // connection dropped
#ifdef LINK_TRACES
		LogVText(LINK_MODULE, 8, true, "received %d bytes", last);
#endif

		total += last;
	}
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 8, true, "received %d total bytes", total);
#endif

	return true;
}

/**
 * \fn bool Link::Receive(unsigned char *byte_buffer, unsigned int *data_lenP)
 * \brief Receives up to data_len bytes into byte_buffer from the linked peer.
 *        This a blocking potentially non complete receive.
 *
 *	\param byte_buffer points to the reception buffer
 *	\param data_len is the max amount of data to be received
 *	\return TRUE if data could be received, else returns FALSE.
 */
bool Link::Receive(unsigned char *byte_buffer, int *data_lenP) {
	if (!byte_buffer || !data_lenP || m_in == -1)
		return false;
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 0, true, "Link::Receive(%p, %d)", byte_buffer, *data_lenP);
#endif

	*data_lenP = recv(m_in, (char *)byte_buffer, *data_lenP, MSG_NOSIGNAL);
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 8, true, "received %d total bytes", *data_lenP);
#endif

	return *data_lenP > 0;
}

/**
 * \fn bool Link::Peek(unsigned char *byte_buffer, unsigned int *data_lenP)
 * \brief Peeks up to data_len bytes into byte_buffer from the linked peer.
 *        This a blocking potentially non complete receive.
 *
 *	\param byte_buffer points to the reception buffer
 *	\param data_len is the max amount of data to be received
 *	\return TRUE if data could be received, else returns FALSE.
 */
bool Link::Peek(unsigned char *byte_buffer, int *data_lenP) {
	if (!byte_buffer || !data_lenP || m_in == -1)
		return false;
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 0, true, "Link::Peek(%p, %d)", byte_buffer, *data_lenP);
#endif

	*data_lenP = recv(m_in, (char *)byte_buffer, *data_lenP, MSG_NOSIGNAL | MSG_PEEK);
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 8, true, "peeked %d total bytes", *data_lenP);
#endif

	return *data_lenP > 0;
}
