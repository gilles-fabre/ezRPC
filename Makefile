install: rebuild
	sudo cp *.so /usr/lib/
	sudo cp Thread/*.h /usr/include
	sudo cp Transport/*.h /usr/include
	sudo cp RPC/*.h /usr/include
	sudo cp StateMachine/*.h /usr/include

Thread :
	make -C Thread
	
Transport :
	make -C Transport

RPC : Thread Transport
	make -C RPC
	make -C RPC -f Makefile.test rebuild

StateMachine : RPC
	make -C StateMachine

EClient : StateMachine
	make -C EClient

EPE : EClient
	make -C EPE
	make -C EPE -f Makefile.test rebuild

rebuild :
	make -C Thread rebuild
	make -C Transport rebuild
	make -C RPC rebuild
	make -C StateMachine rebuild
	make -C EClient rebuild
	make -C EPE rebuild

clean :
	make -C Thread clean
	make -C Transport clean
	make -C RPC clean
	make -C StateMachine clean
	make -C EClient clean
	make -C EPE clean

all : rebuild
