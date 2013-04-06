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

int backlog = 100;

int init_listener()
{
  int sock_fd;
  struct addrinfo hints, *result, *rp;
  
  int port;
  sscanf(get_config("port"),"%d",&port);
  
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
    
    printf("Created socket...\n");
    
    if (bind(sock_fd,rp->ai_addr,rp->ai_addrlen) == 0)
    {
      printf("Bound socket on to address %s...\n",rp->ai_addr->sa_data);
      break;
    } else {
      printf("Failed to bind socket.\n");
    }
    
    close(sock_fd);
  }
  
  if (listen(sock_fd, backlog) == -1)
    error(1,errno,"Error listening on socket. See %s, line %d.", __FILE__, __LINE__);
  
  printf("Initted listener on port %d with fd %d.\n",port,sock_fd);
      
  freeaddrinfo(result);
  return sock_fd;
}