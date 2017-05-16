CXX = g++
CXX_FLAGS = -std=c++11 -fPIC -ggdb
LXX_FLAGS = -l pthread
OBJECTS := $(patsubst %.cpp,%.o, $(wildcard *.cpp))
SHARED_LIBS := $(wildcard *.so)

all : Thread

Thread.o : Thread.cpp Thread.h
	$(CXX) -c Thread.cpp $(CXX_FLAGS)  

Thread : Thread.o 
	$(CXX) -o libThread.so -shared Thread.o $(CXX_FLAGS) $(LXX_FLAGS)
	-cp libThread.so ../

rebuild : clean all 

clean :
	-rm ${OBJECTS} ${SHARED_LIBS} ../${SHARED_LIBS} 2>/dev/null

