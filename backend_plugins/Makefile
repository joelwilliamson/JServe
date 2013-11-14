cxx = g++
CXXFLAGS += -Wall -std=c++11 -fpic -shared
default_backend: default_backend.cpp plugin_types.h ../jserve
	${CXX} ${CXXFLAGS} -o default_backend.so -C default_backend.cpp ../*.o
