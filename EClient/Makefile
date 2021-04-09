CXX = g++
CXX_FLAGS = -std=c++11 -fPIC -ggdb
LXX_FLAGS = -L ../Transport -l Transport -L ../Thread -l Thread -L ../RPC -l RPC
IXX_FLAGS = -I ../Transport -I ../Thread -I ../RPC 
OBJECTS := $(patsubst %.cpp,%.o, $(wildcard *.cpp))
SHARED_LIBS := $(wildcard *.so)

all : EClient 

EClient.o : EClient.cpp EClient.h 
	$(CXX) -c EClient.cpp $(CXX_FLAGS) $(IXX_FLAGS) 

EClient : EClient.o 
	$(CXX) -o libEClient.so -shared EClient.o $(CXX_FLAGS) $(LXX_FLAGS)
	-cp libEClient.so ../

rebuild : clean all  

clean :
	-rm -rf ${OBJECTS} ${SHARED_LIBS} ../${SHARED_LIBS} 2>/dev/null
	

	
