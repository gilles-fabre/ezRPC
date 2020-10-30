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

rebuild :
	make -C Thread rebuild
	make -C Transport rebuild
	make -C RPC rebuild
	make -C StateMachine rebuild

clean :
	make -C Thread clean
	make -C Transport clean
	make -C RPC clean
	make -C StateMachine clean

all : rebuild