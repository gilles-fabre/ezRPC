#CXX_FLAGS = -fPIC -ggdb -m32
#CXX_FLAGS = -fPIC -O3 -m32
CXX_FLAGS = -fPIC -ggdb 
#CXX_FLAGS = -fPIC -O3

OBJECTS := $(patsubst %.cpp,%.o, $(wildcard *.cpp))
SHARED_LIBS := $(wildcard *.so)

all : Transport 

Link.o : Link.cpp Link.h
	$(CXX) -c Link.cpp $(CXX_FLAGS)

Transport.o : Transport.cpp Transport.h Link.h
	$(CXX) -c Transport.cpp $(CXX_FLAGS)

Transport : $(OBJECTS)
	$(CXX) -o libTransport.so -shared Link.o Transport.o $(CXX_FLAGS)
	-cp libTransport.so ../

rebuild : clean Transport

clean :
	-rm -rf ${OBJECTS} ${SHARED_LIBS} ../${SHARED_LIBS} 2>/dev/null
