#include <sys/socket.h>
#include <bits/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>


/* Creates a socket to listen for new connections on.
 * The function returns an int that can be used as a
 * descriptor for the socket, or -1 if anything fails.
 */
int init( ... )
{
  int sd; //Socket descriptor
  struct sockaddr addr;
  
  sd = socket(AF_INET6, SOCK_STREAM, 0);
  if (sd == -1) {
    error(1,errno,"Socket failed to open.\n");
  }
  
  memset(&addr,0,sizeof(addr)); // Zero out the entire addr struct
  addr.
  
  if (!bind(...)) {
    error(1,errno, "Socket failed to bind.\n");
  }
  
}