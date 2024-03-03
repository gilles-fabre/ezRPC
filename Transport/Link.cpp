#include "Link.h"

/**
 * \fn bool Link::Send(unsigned char* byteBuffer, unsigned int dataLen)
 * \brief Sends dataLen bytes from byteBuffer to the linked peer.
 *        This a blocking complete send.
 *
 *	\param byteBuffer points to the data to be sent
 *	\param dataLen is the amount of data to be sent
 *	\return TRUE if all data could be sent, else returns FALSE.
 */
ReturnValue<bool, CommunicationErrors>&& Link::Send(unsigned char* byteBuffer, unsigned int dataLen) {
	ReturnValue<bool, CommunicationErrors> r;
	int									   total = 0,
									       last;

	if (!byteBuffer || dataLen == 0 || m_out == -1) {
		r = ReturnValue<bool, CommunicationErrors>{ false, CommunicationErrors::ErrorCode::BadArgument };
		return std::move(r);
	}
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 0, true, "Link::Send(%p, %d)", byteBuffer, dataLen);
#endif

	while (total != dataLen) {
		if ((last = send(m_out, (const char *)&byteBuffer[total], dataLen - total, MSG_NOSIGNAL)) <= 0) {
			r = ReturnValue<bool, CommunicationErrors>{ false, CommunicationErrors::ErrorCode::CommunicationDropped };
			return std::move(r);
		}

#ifdef LINK_TRACES
		LogVText(LINK_MODULE, 8, true, "sent %d bytes", last);
#endif

		total += last;
	}
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 8, true, "sent %d total bytes", total);
#endif

	r = ReturnValue<bool, CommunicationErrors>{ true, CommunicationErrors::ErrorCode::None };
	return std::move(r);
}

/**
 * \fn bool Link::Send(unsigned char* byteBuffer, unsigned int* dataLenP)
 * \brief Sends up to dataLen bytes from byteBuffer to the linked peer.
 *        This a blocking potentially non complete send.
 *
 *	\param byteBuffer points to the data to be sent
 *	\param dataLen is the max amount of data to be sent
 *	\return TRUE if data could be sent, else returns FALSE.
 */
ReturnValue<bool, CommunicationErrors>&& Link::Send(unsigned char* byteBuffer, unsigned int* dataLenP) {
	ReturnValue<bool, CommunicationErrors> r;

	if (!byteBuffer || !dataLenP || *dataLenP == 0 || m_out == -1) {
		r = ReturnValue<bool, CommunicationErrors>{ false, CommunicationErrors::ErrorCode::BadArgument };
		return std::move(r);
	}
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 0, true, "Link::Send(%p, %d)", byteBuffer, *dataLenP);
#endif

	*dataLenP = send(m_out, (const char *)byteBuffer, *dataLenP, MSG_NOSIGNAL);
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 8, true, "sent %d total bytes", *dataLenP);
#endif

	bool allSent = *dataLenP > 0;
	r = ReturnValue<bool, CommunicationErrors>{ allSent , allSent ? CommunicationErrors::ErrorCode::None : CommunicationErrors::ErrorCode::MissingData };
	return std::move(r);
}

/**
 * \fn bool Link::Receive(unsigned char* byteBuffer, unsigned int dataLen)
 * \brief Receives dataLen bytes into byteBuffer from the linked peer.
 *        This a blocking complete receive.
 *
 *	\param byteBuffer points to the reception buffer
 *	\param dataLen is the amount of data to be received
 *	\return TRUE if all data could be received, else returns FALSE.
 */
ReturnValue<bool, CommunicationErrors>&& Link::Receive(unsigned char* byteBuffer, unsigned int dataLen) {
	ReturnValue<bool, CommunicationErrors> r;
	int									   total = 0,
										   last;

	if (!byteBuffer || dataLen == 0 || m_in == -1) {
		r = ReturnValue<bool, CommunicationErrors>{ false, CommunicationErrors::ErrorCode::BadArgument };
		return std::move(r);
	}
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 0, true, "Link::Receive(%p, %d)", byteBuffer, dataLen);
#endif

	while (total != dataLen) {
		if ((last = recv(m_in, (char*)&byteBuffer[total], dataLen - total, MSG_NOSIGNAL)) <= 0) {
			r = ReturnValue<bool, CommunicationErrors>{ false, CommunicationErrors::ErrorCode::CommunicationDropped };
			return std::move(r);
		}

#ifdef LINK_TRACES
		LogVText(LINK_MODULE, 8, true, "received %d bytes", last);
#endif

		total += last;
	}
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 8, true, "received %d total bytes", total);
#endif

	r = ReturnValue<bool, CommunicationErrors>{ true, CommunicationErrors::ErrorCode::None };
	return std::move(r);
}

/**
 * \fn bool Link::Receive(unsigned char* byteBuffer, unsigned int* dataLenP)
 * \brief Receives up to dataLen bytes into byteBuffer from the linked peer.
 *        This a blocking potentially non complete receive.
 *
 *	\param byteBuffer points to the reception buffer
 *	\param dataLen is the max amount of data to be received
 *	\return TRUE if data could be received, else returns FALSE.
 */
ReturnValue<bool, CommunicationErrors>&& Link::Receive(unsigned char* byteBuffer, unsigned int* dataLenP) {
	ReturnValue<bool, CommunicationErrors> r;

	if (!byteBuffer || !dataLenP || m_in == -1) {
		r = ReturnValue<bool, CommunicationErrors>{ false, CommunicationErrors::ErrorCode::BadArgument };
		return std::move(r);
	}
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 0, true, "Link::Receive(%p, %d)", byteBuffer, *dataLenP);
#endif

	*dataLenP = recv(m_in, (char *)byteBuffer, *dataLenP, MSG_NOSIGNAL);
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 8, true, "received %d total bytes", *dataLenP);
#endif

	bool allReceived = *dataLenP > 0;
	r = ReturnValue<bool, CommunicationErrors>{ allReceived , allReceived ? CommunicationErrors::ErrorCode::None : CommunicationErrors::ErrorCode::MissingData };
	return std::move(r);
}

/**
 * \fn bool Link::Peek(unsigned char* byteBuffer, unsigned int* dataLenP)
 * \brief Peeks up to dataLen bytes into byteBuffer from the linked peer.
 *        This a blocking potentially non complete receive.
 *
 *	\param byteBuffer points to the reception buffer
 *	\param dataLen is the max amount of data to be received
 *	\return TRUE if data could be received, else returns FALSE.
 */
ReturnValue<bool, CommunicationErrors>&& Link::Peek(unsigned char* byteBuffer, unsigned int* dataLenP) {
	ReturnValue<bool, CommunicationErrors> r;

	if (!byteBuffer || !dataLenP || *dataLenP == 0 || m_in == -1) {
		r = ReturnValue<bool, CommunicationErrors>{ false, CommunicationErrors::ErrorCode::BadArgument };
		return std::move(r);
	}
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 0, true, "Link::Peek(%p, %d)", byteBuffer, *dataLenP);
#endif

	*dataLenP = recv(m_in, (char *)byteBuffer, *dataLenP, MSG_NOSIGNAL | MSG_PEEK);
#ifdef LINK_TRACES
	LogVText(LINK_MODULE, 8, true, "peeked %d total bytes", *dataLenP);
#endif

	bool allReceived = *dataLenP > 0;
	r = ReturnValue<bool, CommunicationErrors>{ allReceived , allReceived ? CommunicationErrors::ErrorCode::None : CommunicationErrors::ErrorCode::MissingData };
	return std::move(r);
}
