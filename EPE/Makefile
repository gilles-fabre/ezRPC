CXX = g++
CXX_FLAGS = -std=c++11 -fPIC -ggdb
LXX_FLAGS = -L ../Transport -L ../Thread -L ../RPC -L ../StateMachine -l Transport -l Thread -l RPC -l pthread -l StateMachine
IXX_FLAGS = -I ../Transport -I ../RPC -I ../Thread -I ../StateMachine
OBJECTS := $(patsubst %.cpp,%.o, $(wildcard *.cpp))

all : EPE

EPE :
	$(CXX) -o EPE EPE.cpp $(CXX_FLAGS) $(IXX_FLAGS) $(LXX_FLAGS)
	 
clean :
	-rm -rf EPE ${OBJECTS} 2>/dev/null

rebuild : clean EPE

	

	
