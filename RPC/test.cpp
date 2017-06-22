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
 * \file test.cpp
 *
 * \date Mar 16, 2017
 * \author gilles-fabre
 **/

#include <iostream>
#include <string>
#include <vector>

#include <Transport.h>

#include "RemoteProcedureCall.h"
#include "RPCServer.h"
#include "RPCClient.h"

using namespace std;

RPCServer *gRpcServerP = NULL;

static unsigned long ByeBye(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	if (gRpcServerP)
		gRpcServerP->Stop();
}

static unsigned long Nop(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	return (unsigned long)0;
}

static unsigned long IncDouble(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	double &d = p1->GetDoubleValue();
	cout << "value: " << d << endl;
	d += 0.1;
	cout << "inc'ed value: " << d << endl;
	return (unsigned long)0;
}

static unsigned long Increment(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	int16_t &i = p1->GetInt16Value();
	cout << "value: " << i << endl;
	++i;
	cout << "inc'ed value: " << i << endl;
	return (unsigned long)i;
}

static unsigned long Concatenate(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	RemoteProcedureCall::Parameter *p2 = (*v)[2];


	string &text = p1->GetStringValue();
	cout << "text: " << text << endl;
	string origin_text = text;
	cout << "origin_text: " << origin_text << endl;
	int16_t num = p2->GetInt16Value();
	cout << "num concat: " << num << endl;
	for (int i = 0; i < num; i++)
		text.append(origin_text);
	cout << "concatenated text: " << text << endl;
	return text.length();
}

static unsigned long RepeatPrint(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	RemoteProcedureCall::Parameter *p2 = (*v)[2];

	string text;
	text = p1->GetStringValue();
	int16_t num = p2->GetInt16Value();

	for (int i = 0; i < num; i++)
		cout << "repeat: " << text << endl;

	return num;
}

static unsigned long SumNumbers(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	RemoteProcedureCall::Parameter *p2 = (*v)[2];

	int16_t num1 = p1->GetInt16Value();
	int16_t num2 = p2->GetInt16Value();

	cout << "sum: " << num1 + num2 << endl;

	return num1 + num2;
}

int main(int argc, char **argv) {
	if (argc < 4) {
		cout << "usage:" << endl;
		cout << "\ttest <tcp|file> server server_address" << endl;
		cout << "\ttest <tcp|file> client server_address function_name [repeat_count]" << endl;
		cout << "\ttest server_address must be addr:port for tcp" << endl;
		return -1;
	}

	cout << "argc :" << argc << endl;
	for (int i = 0; i < argc; i++)
		cout << "arg #" << i << ": " << argv[i] << endl;

	string proto = argv[1];
	string what = argv[2];
	string server_addr = argv[3];

	if (what == "server") {
		RPCServer server(proto == "tcp" ? Transport::TCP : Transport::FILE, server_addr);
		gRpcServerP = &server;

		server.RegisterProcedure("nop", &Nop);
		server.RegisterProcedure("inc", &Increment);
		server.RegisterProcedure("repeat", &RepeatPrint);
		server.RegisterProcedure("concat", &Concatenate);
		server.RegisterProcedure("sum", &SumNumbers);
		server.RegisterProcedure("incdouble", &IncDouble);
		server.RegisterProcedure("byebye", &ByeBye);


		server.IterateAndWait();
	} else if (what == "client"){
		RPCClient client(proto == "tcp" ? Transport::TCP : Transport::FILE, server_addr);

		if (argc < 5) {
			cout << "usage:" << endl;
			cout << "\ttest client <tcp|file> server_addr func_name" << endl;
			return -1;
		}

		string func_name = argv[4];

		unsigned long result = -1;
		if (func_name == "nop") {
			if (argc < 5) {
				cout << "usage:" << endl;
				cout << "\ttest client <tcp|file> server_addr nop" << endl;
				return -1;
			}
			int16_t repeat = argc == 6 ? atoi(argv[5]) : 1;
			for (int b = 0; b < repeat; b++) {
				result = client.RpcCall(func_name,
										RemoteProcedureCall::END_OF_CALL);
			}
			cout << "result value: " << result << endl;
		} else if (func_name == "inc") {
			if (argc < 6) {
				cout << "usage:" << endl;
				cout << "\ttest client <tcp|file> server_addr inc value" << endl;
				return -1;
			}
			int16_t repeat = argc == 7 ? atoi(argv[6]) : 1;
			int16_t i;
			for (int b = 0; b < repeat; b++) {
				i = atoi(argv[5]);
				result = client.RpcCall(func_name,
										RemoteProcedureCall::PTR,
										RemoteProcedureCall::INT16,
										&i,
										RemoteProcedureCall::END_OF_CALL);
			}
			cout << "result value: " << i << endl;
		} else if (func_name == "incdouble") {
			if (argc < 6) {
				cout << "usage:" << endl;
				cout << "\ttest client <tcp|file> server_addr incdouble float_value" << endl;
				return -1;
			}
			int16_t repeat = argc == 7 ? atoi(argv[6]) : 1;
			double d;
			for (int b = 0; b < repeat; b++) {
				d = atof(argv[5]);
				result = client.RpcCall(func_name,
										RemoteProcedureCall::PTR,
										RemoteProcedureCall::DOUBLE,
										&d,
										RemoteProcedureCall::END_OF_CALL);
			}
			cout << "result value: " << d << endl;
		} else if (func_name == "concat") {
			if (argc < 7) {
				cout << "usage:" << endl;
				cout << "\ttest client <tcp|file> server_addr concat string num_concat" << endl;
				return -1;
			}
			int16_t repeat = argc == 8 ? atoi(argv[7]) : 1;
			string p;
			for (int b = 0; b < repeat; b++) {
				p = argv[5];
				result = client.RpcCall(func_name,
										RemoteProcedureCall::PTR,
										RemoteProcedureCall::STRING,
										&p,
										RemoteProcedureCall::INT16,
										atoi(argv[6]),
										RemoteProcedureCall::END_OF_CALL);
			}
			cout << "result string: " << p << endl;
		} else if (func_name == "repeat") {
			if (argc < 7) {
				cout << "usage:" << endl;
				cout << "\ttest client <tcp|file> server_addr repeat string num_repeat" << endl;
				return -1;
			}
			int16_t repeat = argc == 8 ? atoi(argv[7]) : 1;
			string s;
			for (int b = 0; b < repeat; b++) {
				s = argv[5];
				result = client.RpcCall(func_name,
										RemoteProcedureCall::STRING,
										&s,
										RemoteProcedureCall::INT16,
										atoi(argv[6]),
										RemoteProcedureCall::END_OF_CALL);
			}
		} else if (func_name == "sum") {
			if (argc < 7) {
				cout << "usage:" << endl;
				cout << "\ttest client <tcp|file> server_addr sum number1 number2" << endl;
				return -1;
			}
			int16_t repeat = argc == 8 ? atoi(argv[7]) : 1;
			for (int b = 0; b < repeat; b++) {
				result = client.RpcCall(func_name,
										RemoteProcedureCall::INT16,
										atoi(argv[5]),
										RemoteProcedureCall::INT16,
										atoi(argv[6]),
										RemoteProcedureCall::END_OF_CALL);
			}
			cout << "result : " << result << endl;
		} else if (func_name == "byebye") {
			result = client.RpcCall(func_name, RemoteProcedureCall::END_OF_CALL);
		}

		cout << "rpc server has returned \"" << result << "\"" << endl;
	}

	return 0;
}




