CXX = g++
CXXFLAGS += -Wall -MMD -std=c++11
EXEC = jserve
LIBS = -ldl
OBJECTS = backend_manager.o configuration.o handler.o request.o init_listener.o logger.o main.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC} ${LIBS}

-include ${DEPENDS}

clean:
	rm ${OBJECTS} ${EXEC} ${DEPENDS}

.PHONY: clean
