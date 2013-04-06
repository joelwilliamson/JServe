DFAULT_CFLAGS = -Wall
all: jserve

memcheck: jserve
	valgrind --leak-check=full --show-reachable=yes ./jserve --p 54321 -r .

jserve: spawn_handler.o init_listener.o main.c configuration.o cate.o
	cc $(DEFAULT_CFLAGS) $(CFLAGS) -o jserve main.c spawn_handler.o init_listener.o configuration.o cate.o -pthread
	
init_listener.o : init_listener.h init_listener.c
	cc $(DEFAULT_CFLAGS) $(CFLAGS) -o init_listener.o -c init_listener.c
	
spawn_handler.o: spawn_handler.h spawn_handler.c
	cc $(DEFAULT_CFLAGS) $(CFLAGS) -o spawn_handler.o -c spawn_handler.c 

configuration.o: configuration.c configuration.h
	cc $(DEFAULT_CFLAGS) $(CFLAGS) -o configuration.o -c configuration.c -DCONFIG_FILE=\"./jserve.conf\"
	
cate.o : convert_ascii_to_ebcdic.c convert_ascii_to_ebcdic.h
	cc $(DEFAULT_CFLAGS) $(CFLAGS) -o cate.o -c convert_ascii_to_ebcdic.c
	
clean:
	rm -v *.o jserve