#include "spawn_handler.hpp"

#include "configuration.h"
#include "backend_manager.hpp"
#include "logger.hpp"

#include <unistd.h>
#include <error.h>
#include <stdio.h>
#include <cstring>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
//#include <sys/types.h>


#include <string>
#include <sstream>


/* The maximum allowed size for client requests. This is small enough that it
 * might cause issues with long URIs or Entity-Body's
 */
int client_request_size=1024;

extern Log main_log;

struct Request {
  std::string method;
  std::string URI;
  std::string http_version;
  std::string raw;
};


/* The server has encountered an error. Let the client know and add it to the logs. */
int server_error(int client_fd, int err_code)
{
	char err_msg[32];
	switch (err_code) {
		case 400: strcpy(err_msg,"Bad Request"); break;
		case 404: strcpy(err_msg,"File Not Found"); break;
		case 500: strcpy(err_msg,"Internal Server Error"); break;
	}
		
	main_log(std::string("Couldn't handle client request: ") + std::to_string(err_code) + err_msg);
	std::string status_line;
	std::string response_body("<html><head><title>");
	response_body += std::to_string(err_code) + " " + err_msg + "</title></head>";
	response_body += "<body><h1>" + std::to_string(err_code) + " " + err_msg + "</h1></body></html>";
	status_line += "HTTP/1.1 " + std::to_string(err_code) + err_msg + "\r\n" +
			"Content-Length: " + std::to_string(response_body.size()) + "\r\n" +
			"Connection: close\r\n" +
			"\r\n" +
			response_body + "\r\n";
	write(client_fd,status_line.c_str(),status_line.size());
	main_log("Sent:\n" + status_line);
	return err_code;
}

/* Read the request from the socket, returning in a nice binary format.
 * The caller is reponsible for freeing the heap allocated return structure.
 * If something goes wrong, a null pointer is returned.
 */
Request *read_request(int fd)
{
	Request *retval = new Request;

	char * client_request = new char[client_request_size+1];
	int amount_read;
	/* Get the request from the socket. */
	if ((amount_read = read(fd,client_request,client_request_size))==-1) {
		server_error(fd,500);
		return NULL;
	} else {
		client_request[amount_read]=0; // Make sure we have a valid C string
	}
	main_log(std::string("Read Client Request:") + client_request,DEBUG);
	
	/* Parse the request header */
	std::stringstream request_stream(client_request);
	request_stream >> retval->method;
	request_stream >> retval->request_uri;
	request_stream >> retval->http_version;
	if (request_stream.fail()) {
		main_log(std::string("Received a bad request:\n")+client_request,WARNING);
		server_error(fd,400);
		return NULL;
	} else {
		main_log("Method: " + retval->method + "\nRequest URI: " + retval->request_uri + "\nHTTP Version: " + retval->http_version + "\n");
	}
	
	retval->request_data = client_request;
	
	return retval;
}


/* Send the buffer data to the socket fd.
 * Return 0 on success, non-zero on failure.
 */
int send_data(int fd, const char* data)
{
	int body_length = strlen(data);
	
	char *response;
	asprintf(&response,
		"HTTP/1.1 200 OK\r\n" /* Status Line */
		"Content-Type: text/html; charset=utf-8\r\n"
		"Content-Length: %d\r\n"
		"Connection: close\r\n"
		"\r\n"
		"%s",body_length,data);
  
	write(fd,response,strlen(response));
	main_log("Sent Message:",DEBUG);
	main_log(response,DEBUG);
	free(response);
	return 0;
}

/* Does the actual work of handling the client. This allows spawn_handler to
 * create a new thread and immediately return.
 */
void handler(int fd)
	{
	Request *req;
	
	main_log << DEBUG << "Reading request...";
	req = read_request(fd);
	
	if (!req) {
		main_log << WARNING << "Server error occured. Ending handler...";
		return;
	} else main_log << DEBUG <<"Finished reading request.";
  
  
  
	std::string file_path(get_config("root_dir"));
	file_path=file_path + "/" + req->request_uri;
	
	int error_code;
	std::string response_body = get_resource(file_path,NULL,error_code);
	
	if (!response_body.size()) {
		main_log << ERROR << "Server error occurred. Ending handler...\n";
		return;
		}
  
	if (send_data(fd,response_body.c_str())) {
		main_log << WARNING << "Server error occured. Ending handler...\n";
		return;
		}
  
	delete req;
	return;
	}


int spawn_handler(int fd)
	{
	std::thread(handler,fd).detach();
	}
