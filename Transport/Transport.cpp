/*
                     GNU LESSER GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.


  This version of the GNU Lesser General Public License incorporates
the terms and conditions of version 3 of the GNU General Public
License, supplemented by the additional permissions listed below.

  0. Additional Definitions.

  As used herein, "this License" refers to version 3 of the GNU Lesser
General Public License, and the "GNU GPL" refers to version 3 of the GNU
General Public License.

  "The Library" refers to a covered work governed by this License,
other than an Application or a Combined Work as defined below.

  An "Application" is any work that makes use of an interface provided
by the Library, but which is not otherwise based on the Library.
Defining a subclass of a class defined by the Library is deemed a mode
of using an interface provided by the Library.

  A "Combined Work" is a work produced by combining or linking an
Application with the Library.  The particular version of the Library
with which the Combined Work was made is also called the "Linked
Version".

  The "Minimal Corresponding Source" for a Combined Work means the
Corresponding Source for the Combined Work, excluding any source code
for portions of the Combined Work that, considered in isolation, are
based on the Application, and not on the Linked Version.

  The "Corresponding Application Code" for a Combined Work means the
object code and/or source code for the Application, including any data
and utility programs needed for reproducing the Combined Work from the
Application, but excluding the System Libraries of the Combined Work.

  1. Exception to Section 3 of the GNU GPL.

  You may convey a covered work under sections 3 and 4 of this License
without being bound by section 3 of the GNU GPL.

  2. Conveying Modified Versions.

  If you modify a copy of the Library, and, in your modifications, a
facility refers to a function or data to be supplied by an Application
that uses the facility (other than as an argument passed when the
facility is invoked), then you may convey a copy of the modified
version:

   a) under this License, provided that you make a good faith effort to
   ensure that, in the event an Application does not supply the
   function or data, the facility still operates, and performs
   whatever part of its purpose remains meaningful, or

   b) under the GNU GPL, with none of the additional permissions of
   this License applicable to that copy.

  3. Object Code Incorporating Material from Library Header Files.

  The object code form of an Application may incorporate material from
a header file that is part of the Library.  You may convey such object
code under terms of your choice, provided that, if the incorporated
material is not limited to numerical parameters, data structure
layouts and accessors, or small macros, inline functions and templates
(ten or fewer lines in length), you do both of the following:

   a) Give prominent notice with each copy of the object code that the
   Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the object code with a copy of the GNU GPL and this license
   document.

  4. Combined Works.

  You may convey a Combined Work under terms of your choice that,
taken together, effectively do not restrict modification of the
portions of the Library contained in the Combined Work and reverse
engineering for debugging such modifications, if you also do each of
the following:

   a) Give prominent notice with each copy of the Combined Work that
   the Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the Combined Work with a copy of the GNU GPL and this license
   document.

   c) For a Combined Work that displays copyright notices during
   execution, include the copyright notice for the Library among
   these notices, as well as a reference directing the user to the
   copies of the GNU GPL and this license document.

   d) Do one of the following:

       0) Convey the Minimal Corresponding Source under the terms of this
       License, and the Corresponding Application Code in a form
       suitable for, and under terms that permit, the user to
       recombine or relink the Application with a modified version of
       the Linked Version to produce a modified Combined Work, in the
       manner specified by section 6 of the GNU GPL for conveying
       Corresponding Source.

       1) Use a suitable shared library mechanism for linking with the
       Library.  A suitable mechanism is one that (a) uses at run time
       a copy of the Library already present on the user's computer
       system, and (b) will operate properly with a modified version
       of the Library that is interface-compatible with the Linked
       Version.

   e) Provide Installation Information, but only if you would otherwise
   be required to provide such information under section 6 of the
   GNU GPL, and only to the extent that such information is
   necessary to install and execute a modified version of the
   Combined Work produced by recombining or relinking the
   Application with a modified version of the Linked Version. (If
   you use option 4d0, the Installation Information must accompany
   the Minimal Corresponding Source and Corresponding Application
   Code. If you use option 4d1, you must provide the Installation
   Information in the manner specified by section 6 of the GNU GPL
   for conveying Corresponding Source.)

  5. Combined Libraries.

  You may place library facilities that are a work based on the
Library side by side in a single library together with other library
facilities that are not Applications and are not covered by this
License, and convey such a combined library under terms of your
choice, if you do both of the following:

   a) Accompany the combined library with a copy of the same work based
   on the Library, uncombined with any other library facilities,
   conveyed under the terms of this License.

   b) Give prominent notice with the combined library that part of it
   is a work based on the Library, and explaining where to find the
   accompanying uncombined form of the same work.

  6. Revised Versions of the GNU Lesser General Public License.

  The Free Software Foundation may publish revised and/or new versions
of the GNU Lesser General Public License from time to time. Such new
versions will be similar in spirit to the present version, but may
differ in detail to address new problems or concerns.

  Each version is given a distinguishing version number. If the
Library as you received it specifies that a certain numbered version
of the GNU Lesser General Public License "or any later version"
applies to it, you have the option of following the terms and
conditions either of that published version or of any later version
published by the Free Software Foundation. If the Library as you
received it does not specify a version number of the GNU Lesser
General Public License, you may choose any version of the GNU Lesser
General Public License ever published by the Free Software Foundation.

  If the Library as you received it specifies that a proxy can decide
whether future versions of the GNU Lesser General Public License shall
apply, that proxy's public statement of acceptance of any version is
permanent authorization for you to choose that version for the
Library.
*/

/**
 * \file Transport.cpp
 *
 * \author gilles fabre
 * \date Mar 10, 2017
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h> // PF_INET/AF_INET
#include <arpa/inet.h>

#include <sys/un.h>		// AF_UNIX

#include <sys/types.h>  // mkfifo
#include <sys/stat.h>   // mkfifo
#include <fcntl.h>

#include "Transport.h"

using namespace std;

/**
 * \fn Transport *Transport::CreateTransport(TransportType transport_type)
 * \brief Returns a newly created transport for the given transport type
 *
 * \param transport_type specifies the requested type of underlying transport
 * \return an instance of specialized transport, NULL if an error occured
 */
Transport *Transport::CreateTransport(TransportType transport_type) {
	Transport *transport = NULL;
#ifdef TRANSPORT_TRACES
	cout << "\n Transport::CreateTransport " << endl;
#endif

	switch (transport_type) {
		case TCP:
			transport = new TcpTransport();
#ifdef TRANSPORT_TRACES
			cout << "\t created transport " << transport << " of type TCP" << endl;
#endif
			break;

		case FILE:
			transport = new FileTransport();
#ifdef TRANSPORT_TRACES
			cout << "\t created transport " << transport << " of type FILE" << endl;
#endif
			break;

		default:
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: unknown Transport type!" << endl;
			break;
	}

	return transport;
}

/**
 * \fn Link* TcpTransport::WaitForLinkRequest(const string &server_address)
 * \brief Waits for a LinkRequest and returns the resulting link
 *
 * \param server_address is the IP address to listen on
 * \return a connected link to the peer, NULL upon error
 */
Link* TcpTransport::WaitForLinkRequest(const string &server_address) {
	int 						c_socket;
	struct 	sockaddr_in 		server_addr = {0,};
	struct 	sockaddr_storage 	server_storage;
	socklen_t 					addr_size;
	string						s_host;
	string						s_port;
	int							n_port;
	int 						retval;
	int 						off = 0;

#ifdef TRANSPORT_TRACES
	cout << "\n TcpTransport::WaitForLinkRequest " << server_address << endl;
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

	s_host = server_address.substr(0, found);
	if (s_host.empty())
		s_host = "0.0.0.0";
#ifdef TRANSPORT_TRACES
	cout << "\t found host addr " << s_host << " and port " << n_port << endl;
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
		server_addr.sin_addr.s_addr = inet_addr(s_host.c_str());

		// bind the address struct to the socket
		bind(m_s_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
#ifdef TRANSPORT_TRACES
		cout << "\t created and bound to socket " << m_s_socket << endl;
#endif

		if (setsockopt(m_s_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&off, sizeof(off)) < 0) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't set socket option (" << strerror(errno) << ")" << endl;
		    close(m_s_socket);
		    m_s_socket = -1;
			return NULL;
		}
	}
#ifdef TRANSPORT_TRACES
	cout << "\t will now listen on socket " << m_s_socket << endl;
#endif

	// listen on the socket, with 1 max connection request queued
	if ((retval = listen(m_s_socket, 1))) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't listen on socket (" << strerror(errno) << ")" << endl;
		return NULL;
	}
#ifdef TRANSPORT_TRACES
	cout << "\t accepting connection on socket " << m_s_socket << endl;
#endif

	// accept call creates a new socket for the incoming connection
	addr_size = sizeof(server_storage);
	c_socket = accept(m_s_socket, (struct sockaddr *)&server_storage, &addr_size);

	if (setsockopt(c_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&off, sizeof(off)) < 0) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't set socket option (" << strerror(errno) << ")" << endl;
	    close(c_socket);
		return NULL;
	}

	Link *new_linkP = new Link(c_socket, c_socket);
	m_links.push_back(new_linkP);
#ifdef TRANSPORT_TRACES
	cout << "\t will return link " << new_linkP << endl;
#endif

	return new_linkP;
}

/**
 * \fn Link* TcpTransport::LinkRequest(const string &server_address)
 * \brief Connects to a server blocked on WaitLinkRequest and
 * 		  returns the resulting link
 *
 * \param server_address is the IP addr of the server to connect to
 * \return a connected link to the peer, NULL upon error
 */
Link* TcpTransport::LinkRequest(const string &server_address) {
	int 				c_socket;
	struct 	sockaddr_in server_addr = {0,};
	string				s_host;
	string				s_port;
	int					n_port;
	int 				retval;
#ifdef TRANSPORT_TRACES
	cout << "\n TcpTransport::LinkRequest " << server_address << endl;
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

	s_host = server_address.substr(0, found);
	if (s_host.empty())
		s_host = "0.0.0.0";
#ifdef TRANSPORT_TRACES
	cout << "\t found host addr " << s_host << " and port " << n_port << endl;
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
	server_addr.sin_addr.s_addr = inet_addr(s_host.c_str());
#ifdef TRANSPORT_TRACES
	cout << "\t created and will connect to socket " << c_socket << endl;
#endif

	// connect the socket to the server using the address struct
	if ((retval = connect(c_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1)  {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't connect to socket (" << strerror(errno) << ")" << endl;
		close(c_socket);
		return NULL;
	}

	Link *new_linkP = new Link(c_socket, c_socket);
	m_links.push_back(new_linkP);
#ifdef TRANSPORT_TRACES
	cout << "\t will return link " << new_linkP << endl;
#endif

	return new_linkP;
}

/**
 * \fn Link* FileTransport::WaitForLinkRequest(const string &server_address)
 * \brief Waits for a LinkRequest and returns the resulting link
 *
 * \param server_address is the file name to listen on
 * \return a connected link to the peer, NULL upon error
 */
Link* FileTransport::WaitForLinkRequest(const string &server_address) {
	int 						c_socket;
	sockaddr_un 				server_addr = {0,};
	struct 	sockaddr_storage 	server_storage;
	socklen_t 					addr_size;
	int 						retval;
	int 						off = 0;

#ifdef TRANSPORT_TRACES
	cout << "\n FileTransport::WaitForLinkRequest " << server_address << endl;
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
		if ((m_s_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't create socket (" << strerror(errno) << ")" << endl;
			return NULL;
		}

		// configure settings of the server address struct
		// address family = Internet
		server_addr.sun_family = AF_UNIX;

		// set IP address to localhost
		strcpy(server_addr.sun_path, server_address.c_str());

		// bind the address struct to the socket
		bind(m_s_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
#ifdef TRANSPORT_TRACES
		cout << "\t created and bound to socket " << m_s_socket << endl;
#endif

		if (setsockopt(m_s_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&off, sizeof(off)) < 0) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't set socket option (" << strerror(errno) << ")" << endl;
		    close(m_s_socket);
		    m_s_socket = -1;
			return NULL;
		}
	}
#ifdef TRANSPORT_TRACES
	cout << "\t will now listen on socket " << m_s_socket << endl;
#endif

	// listen on the socket, with 1 max connection request queued
	if ((retval = listen(m_s_socket, 1))) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't listen on socket (" << strerror(errno) << ")" << endl;
		return NULL;
	}
#ifdef TRANSPORT_TRACES
	cout << "\t accepting connection on socket " << m_s_socket << endl;
#endif

	// accept call creates a new socket for the incoming connection
	addr_size = sizeof(server_storage);
	c_socket = accept(m_s_socket, (struct sockaddr *)&server_storage, &addr_size);

	if (setsockopt(c_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&off, sizeof(off)) < 0) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't set socket option (" << strerror(errno) << ")" << endl;
	    close(c_socket);
		return NULL;
	}

	Link *new_linkP = new Link(c_socket, c_socket);
	m_links.push_back(new_linkP);
#ifdef TRANSPORT_TRACES
	cout << "\t will return link " << new_linkP << endl;
#endif

	return new_linkP;
}

/**
 * \fn Link* FileTransport::LinkRequest(const string &server_address)
 * \brief Connects to a server blocked on WaitLinkRequest and
 * 		  returns the resulting link
 *
 * \param server_address is the filename the server is listening on
 * \return a connected link to the peer, NULL upon error
 */
Link* FileTransport::LinkRequest(const string &server_address) {
	int 				c_socket;
	sockaddr_un 		server_addr = {0,};
	string				s_host;
	int 				retval;
#ifdef TRANSPORT_TRACES
	cout << "\n FileTransport::LinkRequest " << server_address << endl;
#endif

	// create the client socket
	if ((c_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't create socket (" << strerror(errno) << ")" << endl;
		return NULL;
	}

	// configure settings of the server address struct
	// address family = Internet
	server_addr.sun_family = AF_UNIX;

	// set IP address to localhost
    strcpy(server_addr.sun_path, server_address.c_str());
#ifdef TRANSPORT_TRACES
	cout << "\t created and will connect to socket " << c_socket << endl;
#endif

	// connect the socket to the server using the address struct
	if ((retval = connect(c_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1)  {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't connect to socket (" << strerror(errno) << ")" << endl;
		close(c_socket);
		return NULL;
	}

	Link *new_linkP = new Link(c_socket, c_socket);
	m_links.push_back(new_linkP);
#ifdef TRANSPORT_TRACES
	cout << "\t will return link " << new_linkP << endl;
#endif

	return new_linkP;
}

