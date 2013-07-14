#include <sys/socket.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>

#include "init_listener.h"
#include "configuration.h"
#include "logger.hpp"

int backlog = 100;
static int listen_socket;
extern Log main_log;

int init_listener()
{
  int sock_fd;
  struct addrinfo hints, *result, *rp;
  
  int port;
  sscanf(get_config("port").c_str(),"%d",&port);
  
  memset(&hints,0,sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
  
  char port_string[16];
  sprintf(port_string,"%d",port);
  
  if ((getaddrinfo(NULL,port_string,&hints,&result))== -1)
    error(1,errno,"Error getting valid sockets. See %s, line %d",__FILE__,__LINE__);
  
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    if ((sock_fd=socket(rp->ai_family,rp->ai_socktype,0))==-1)
      continue;
    
    main_log("Created socket...",NOTICE);
    
    if (bind(sock_fd,rp->ai_addr,rp->ai_addrlen) == 0)
    {
      main_log("Bound socket on to address " + std::to_string((long long int)rp->ai_addr->sa_data),DEBUG);;
      break;
    } else {
      main_log("Failed to bind socket.",WARNING);
    }
    
    close(sock_fd);
  }
  
  if (listen(sock_fd, backlog) == -1)
    error(1,errno,"Error listening on socket. See %s, line %d.", __FILE__, __LINE__);
  
  main_log("Initted listener on port " + std::to_string(port) + " with fd " + std::to_string(sock_fd) + ".",DEBUG);
      
  freeaddrinfo(result);
  listen_socket = sock_fd;
  return sock_fd;
}

void close_listener ()
{
	main_log("Closing listening socket...", DEBUG);
	close(listen_socket);
}
