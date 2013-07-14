#include "handler.hpp"

#include "configuration.h"
#include "backend_manager.hpp"
#include "logger.hpp"
#include "request.hpp"

#include <unistd.h>
#include <error.h>
#include <cerrno>
#include <sys/stat.h>
#include <fcntl.h>

#include <string>
#include <sstream>
#include <memory>

extern Log main_log;

/* The server has encountered an error. Let the client know and add it to the logs. */
int server_error ( int client_fd, int err_code )
	{
	std::string err_msg;
	switch ( err_code )
		{
		case 400:
			err_msg = "Bad Request";
			break;
		case 404:
			err_msg = "File Not Found";
			break;
		case 500:
			err_msg = "Internal Server Error";
			break;
		}

	main_log ( std::string ( "Couldn't handle client request: " ) + std::to_string ( err_code ) + err_msg );
	std::string status_line;
	std::string response_body ( "<html><head><title>" );
	response_body += std::to_string ( err_code ) + " " + err_msg + "</title></head>";
	response_body += "<body><h1>" + std::to_string ( err_code ) + " " + err_msg + "</h1></body></html>";
	status_line += "HTTP/1.1 " + std::to_string ( err_code ) + err_msg + "\r\n" +
	               "Content-Length: " + std::to_string ( response_body.size() ) + "\r\n" +
	               "Connection: close\r\n" +
	               "\r\n" +
	               response_body + "\r\n";
	write ( client_fd,status_line.c_str(),status_line.size() );
	main_log ( "Sent:\n" + status_line );
	return err_code;
	}


/* Send the buffer data to the socket fd.
 * Return 0 on success, non-zero on failure.
 */
int Handler::send_data ( const std::string &data )
	{
	std::stringstream response;
	response << "HTTP/1.1 200 OK\r\n" /* Status Line */
		<<"Content-Type: text/html; charset=utf-8\r\n"
		<< "Content-Length: " << data.size() << "\r\n"
		<< "Connection: close\r\n"
		<< "\r\n"
		<< data;

	write ( file_descriptor,response.str().c_str(), response.gcount() );
	main_log ( "Sent Message:",DEBUG );
	main_log ( response.str(),DEBUG );
	free ( response );
	return 0;
	}

/* Does the actual work of handling the client. This allows spawn_handler to
 * create a new thread and immediately return.
 */
void Handler::worker()
	{
		
	main_log ( "Reading request...",DEBUG );
	std::unique_ptr<Request> req;
	try { req  = std::unique_ptr<Request> (new Request(file_descriptor)); }
	catch (int error_number) {
		main_log ( "Server error occured. Ending handler...",WARNING );
		server_error(file_descriptor,error_number);
		return;
		}

	std::string file_path ( get_config ( "root_dir" ) );
	file_path=file_path + "/" + req->URI;

	int error_code;
	std::string response_body = get_resource ( file_path,NULL,error_code );

	if ( !response_body.size() )
		{
		main_log ( "Server error occurred. Ending handler...\n",ERROR );
		return;
		}

	if ( send_data (response_body ) )
		{
		main_log ( "Server error occured. Ending handler...\n",WARNING );
		return;
		}

	}

Handler::Handler ( int file_descriptor )
	: file_descriptor ( file_descriptor ), client_request_size ( 1024 )
	{
	}

std::unique_ptr<Handler> make_handler(int fd)
{
	return std::unique_ptr<Handler>(new Handler(fd));
}

void spawn_handler(int fd)
{
	std::thread t(make_handler,fd);
	t.detach();
}