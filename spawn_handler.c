#include "spawn_handler.h"

#include "configuration.h"
#include "convert_ascii_to_ebcdic.h"

#include <unistd.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
//#include <sys/types.h>

/* The maximum allowed size for client requests. This is small enough that it
 * might cause issues with long URIs or Entity-Body's
 */
int client_request_size=1024;

struct request {
  char *method;
  char *request_uri;
  char *http_version;
  char *request_data;
};


/* The server has encountered an error. Let the client know and add it to the logs. */
int server_error(int fd, int err_code, char *err_msg)
{
  printf("Couldn't handle client request: %d %s\n",err_code, err_msg);
  char status_line[256];
  char response_body[256];
  sprintf(response_body,"<html><head><title>%d %s</title></head><body><h1>%d %s</h1></body></html>",err_code,err_msg,err_code,err_msg);
  sprintf(status_line,
	  "HTTP/1.1 %d %s\r\n"
	  "Content-Length: %d\r\n"
	  "Connection: close\r\n"
	  "\r\n"
	  "%s\r\n",err_code,err_msg,strlen(response_body),response_body);
  write(fd,status_line,strlen(status_line));
  printf("Sent:\n%s",status_line);
  return err_code;
}

/* Read the request from the socket, returning in a nice binary format.
 * The caller is reponsible for freeing the heap allocated return structure.
 * If something goes wrong, a null pointer is returned.
 */
struct request *read_request(int fd)
{
  struct request *retval = malloc(sizeof(struct request));

  char *client_request = malloc(sizeof(char)*client_request_size);
  if (read(fd,client_request,client_request_size)==-1) {
    server_error(fd,500,"Internal Server Error");
    return NULL;
  }
  
  printf("Read Client Request: %s",client_request);
  

  if (sscanf(client_request,"%ms %ms %ms\r\n",&retval->method,&retval->request_uri,&retval->http_version) != 3) {
    server_error(fd,400,"Bad Request");
    return NULL;
  } else {
    printf("Method: %s\n",retval->method);
    printf("Request URI: %s\n",retval->request_uri);
    printf("HTTP Version: %s\n", retval->http_version);
  }
  
  retval->request_data = client_request;
  
  return retval;
}

/* Return a null-terminated string with the contents of filepath.
 * The string will be dynamically allocated, and must be freed.
 * If an error occurs, a null pointer will be returned.
 */
char *read_requested_file(char* filepath, int fd)
{
  struct stat statbuf;
  if (stat(filepath,&statbuf)==-1) {
    printf("Couldn't find file %s.\n",filepath);
    server_error(fd,404,"Not Found");
    return NULL;
  }
  off_t file_size = statbuf.st_size;
  
  /* Create the Response */
  char *response_body = malloc(sizeof(char)*(file_size+1));
  int rf;
  if ((rf = open(filepath,O_RDONLY)) == -1) {
    server_error(fd,404,"Not Found");
    return NULL;
  }
  
  int bytes_read;
  if ((bytes_read = read(rf,response_body,file_size)) == -1) {
    server_error(fd,500,"Internal Server Error");
    return NULL;
  } else {
    response_body[bytes_read]='\0';
  }
  
  ascii_to_ebcdic(response_body,response_body);
  return response_body;
}

/* Send the buffer data to the socket fd.
 * Return 0 on success, non-zero on failure.
 */
int send_data(int fd, char* data)
{
  int body_length = strlen(data);
  
  char *response;
  asprintf(&response,
	   "HTTP/1.1 200 OK\r\n" /* Status Line */
	   "Content-Type: text/html; charset=cp037\r\n"
	   "Content-Length: %d\r\n"
	   "Connection: close\r\n"
	   "\r\n"
	   "%s",body_length,data);
  
  write(fd,response,strlen(response));
  printf("Sent Message:\n%s\n",response);
  free(response);
  return 0;
}

/* Does the actual work of handling the client. This allows spawn_handler to
 * create a new thread and immediately return.
 */
void* handler(void* arg)
{
  int fd = *(int*)arg;
  free(arg);
  struct request *req;
  
  req= read_request(fd);
  
  if (!req) {
    printf("Server error occured. Ending handler...\n");
    return NULL;
  }
  
  
  
  char *file_path;
  
  asprintf(&file_path,"%s/%s",get_config("root_dir"),req->request_uri);
  
  char *response_body = read_requested_file(file_path,fd);
  
  if (!response_body) {
    printf("Server error occurred. Ending handler...\n");
    return NULL;
  }
  
  if (send_data(fd,response_body)) {
    printf("Server error occured. Ending handler...\n");
    return NULL;
  }
  
  free(file_path);
  free(req->method);
  free(req->request_uri);
  free(req->http_version);
  free(req->request_data);
  free(req);
  free(response_body);
  sleep(5);
  return NULL;
}


int spawn_handler(int fd)
{
  pthread_t t;
  int *fd_p = malloc(sizeof(int));
  *fd_p = fd;
  pthread_create(&t,NULL,handler,fd_p);
  return 0;
}
