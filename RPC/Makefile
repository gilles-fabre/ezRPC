#CXX_FLAGS = -fPIC -ggdb -m32
#CXX_FLAGS = -fPIC -O3 -m32
CXX_FLAGS = -fPIC -ggdb 
#CXX_FLAGS = -fPIC -O3
LXX_FLAGS = -L ../Transport -l Transport -L ../Thread -l Thread
IXX_FLAGS = -I ../Transport -I ../Thread
OBJECTS := $(patsubst %.cpp,%.o, $(wildcard *.cpp))
SHARED_LIBS := $(wildcard *.so)

all : RPC 

RPCClient.o : RPCClient.cpp RPCClient.h 
	$(CXX) -c RPCClient.cpp $(CXX_FLAGS) $(IXX_FLAGS) 

RPCServer.o : RPCServer.cpp RPCServer.h 
	$(CXX) -c RPCServer.cpp $(CXX_FLAGS) $(IXX_FLAGS) 

RemoteProcedureCall.o : RemoteProcedureCall.cpp RemoteProcedureCall.h 
	$(CXX) -c RemoteProcedureCall.cpp $(CXX_FLAGS) $(IXX_FLAGS) 

RPC : RemoteProcedureCall.o RPCServer.o RPCClient.o 
	$(CXX) -o libRPC.so -shared RemoteProcedureCall.o RPCServer.o RPCClient.o $(CXX_FLAGS) $(LXX_FLAGS)
	-cp libRPC.so ../

rebuild : clean all  

clean :
	-rm -rf ${OBJECTS} ${SHARED_LIBS} ../${SHARED_LIBS} 2>/dev/null

