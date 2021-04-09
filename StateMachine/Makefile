#CXX_FLAGS = -fPIC -ggdb -m32
#CXX_FLAGS = -fPIC -O3 -m32
CXX_FLAGS = -fPIC -ggdb 
#CXX_FLAGS = -fPIC -O3
LXX_FLAGS = -L ../Transport -l Transport -l jsoncpp -L ../Thread -l Thread
IXX_FLAGS = -I ../Transport -I ../Thread -I ../RPC -I /usr/include/jsoncpp
OBJECTS := $(patsubst %.cpp,%.o, $(wildcard *.cpp))
SHARED_LIBS := $(wildcard *.so)

all : StateMachine 

StateMachine.o : StateMachine.cpp StateMachine.h 
	$(CXX) -c StateMachine.cpp $(CXX_FLAGS) $(IXX_FLAGS) 

StateMachine : StateMachine.o  
	$(CXX) -o libStateMachine.so -shared StateMachine.o $(CXX_FLAGS) $(LXX_FLAGS)
	-cp libStateMachine.so ../

rebuild : clean all  

clean :
	-rm -rf ${OBJECTS} ${SHARED_LIBS} ../${SHARED_LIBS} 2>/dev/null

