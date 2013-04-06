#include "init_listener.h"
#include "spawn_handler.h"
#include "configuration.h"

#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char* optarg;
char *path_root;

int main(int argc, char *argv[])
{
  /* Check if the user needs help */
  if (argc==2 && !strcmp(argv[1],"--help") || getopt(argc,argv,"h")=='h') {
    printf("Usage: %s -p port -r root_dir\n",argv[0]);
    return 0;
  }
  
  read_config();
  
  struct sockaddr addr;
  socklen_t socklen;
  int port_num;
  
  int listening_socket = init_listener();
  
  
  //char recv_mesg[1024];
  
  printf("Ready to listen on fd %d...\n",listening_socket);
  //while ((recv_val = recvfrom(listening_socket,recv_mesg,sizeof(recv_mesg),0,&addr,&socklen))==-1)
  int cfd;
  while ((cfd=accept(listening_socket,&addr,&socklen))!=-1)
  {
    printf("Spawning handler...");
    spawn_handler(cfd);
    printf("Finished handling request.\n");
  }
  free(path_root);
  return 0;
}