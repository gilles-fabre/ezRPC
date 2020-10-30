CXX_FLAGS = -fPIC -ggdb -m32
#CXX_FLAGS = -fPIC -O3 -m32
#CXX_FLAGS = -fPIC -ggdb 
#CXX_FLAGS = -fPIC -O3
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
	-rm -rf ${OBJECTS} ${SHARED_LIBS} ../${SHARED_LIBS} 2>/dev/null

