install: rebuild
	sudo cp *.so /usr/local/lib/
	 
Thread :
	make -C Thread
	
Transport :
	make -C Transport

RPC : Thread Transport
	make -C RPC
	make -C RPC -f Makefile.test rebuild

rebuild :
	make -C Transport rebuild
	make -C Thread rebuild
	make -C RPC rebuild

clean :
	make -C Transport clean
	make -C Thread clean
	make -C RPC clean
	
