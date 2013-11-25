cxx = g++
CXXFLAGS += -Wall -std=c++11 -fpic -shared

all: default_backend.so echo_backend.so


default_backend.so: default_backend.cpp plugin_types.h ../jserve
	${CXX} ${CXXFLAGS} -o default_backend.so -C default_backend.cpp ../*.o

echo_backend.so: echo_backend.cpp plugin_types.h ../jserve
	${CXX} ${CXXFLAGS} -o echo_backend.so -C echo_backend.cpp ../*.o
