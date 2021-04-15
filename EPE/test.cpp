#include <iostream>
#include <string>
#include <vector>

#include <Thread.h>
#include <RPCServer.h>
#include <EClient.h>

#include "EPE.h"

using namespace std;

int main(int argc, char **argv) {
	for (int i = 0; i < argc; i++)
		cout << "arg #" << i << ": " << argv[i] << endl;

	if (argc < 3) {
		cout << "usage:" << endl;
		cout << "\ttest <tcp|file> server_address client_address" << endl;
		return -1;
	}

	string proto = argv[1];
	string server_addr = argv[2];
	string client_addr = argv[3];

	EClient client(proto == "tcp" ? Transport::TCP : Transport::FILE,
				   server_addr,
				   client_addr);

	client.Run();
	SMID smid = (SMID)client.CreateStateMachine("test_state_machine.json");
	puts("\n\n1. inc\n2. restart\n3. exit");
	while (smid) {
		char c = getchar();
		switch(c) {
			case '1':
				client.DoTransition(smid, "inc");
				puts("\n\n1. inc\n2. restart\n3. exit");
				break;

			case '2':
				client.DoTransition(smid, "restart");
				puts("\n\n1. inc\n2. restart\n3. exit");
				break;

			case '3':
				client.DestroyStateMachine(smid);
				puts("\n\n1. inc\n2. restart\n3. exit");
				smid = 0;
				break;
		}
	}

	// wait for last pending callbacks
	for (int i = 10; i > 0; i--) {
		cout << "waiting " <<  i << " seconds before exiting" << endl;
		sleep(1);
	}

	client.Stop();

	return 0;
}




