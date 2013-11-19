#include "handler.hpp"

#include "configuration.hpp"
#include "backend_manager.hpp"
#include "logger.hpp"
#include "request.hpp"
#include "socket.hpp"

#include <unistd.h>
#include <error.h>
#include <cerrno>
#include <sys/stat.h>
#include <fcntl.h>

#include <sstream>
#include <memory>
#include <cerrno>
#include <cstring>
#include <thread>
#include <functional>

extern Log main_log;

/* The server has encountered an error. Let the client know and add it to the logs. */
int Handler::server_error ( int err_code )
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
	socket.write ( status_line.c_str(),status_line.size() );
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

	main_log << DEBUG << "Attempting to send: " << response.str().c_str();
	size_t bytes_written = socket.write ( response.str().c_str(), response.str().size() );
	if (!bytes_written) {
		main_log << NOTICE << "Failed to write: " << strerror(errno) << "\n";
		return 1;
	}
	return 0;
	}

/* Does the actual work of handling the client. This allows spawn_handler to
 * create a new thread and immediately return.
 */
void Handler::operator()()
	{
		
	main_log << DEBUG << "Reading request...";
	std::unique_ptr<Request> req;
	try { req  = std::unique_ptr<Request> (new Request(socket.getfd())); }
	catch (int error_number) {
		main_log << WARNING << "Error reading request. Ending handler...";
		server_error(error_number);
		return;
		}

	std::string file_path ( get_config ( "root_dir" ) );
	main_log << DEBUG << "Using " << file_path << " as the URI root, and "
		<< req->URI << " as the tail.\n";
	file_path=file_path + "/" + req->URI;

	int error_code;
	std::string response_body;
	try {
		response_body = get_resource ( file_path,NULL,error_code );
		}
	catch ( int e ) {
		server_error ( e );
		}

	if ( !response_body.size() )
		{
		main_log ( "Empty response body. Ending handler...\n",ERROR );
		return;
		}

	if ( send_data (response_body ) )
		{
		main_log ( "Error sending data. Ending handler...\n",WARNING );
		return;
		}

	}

Handler::Handler ( Socket&& s )
	: socket(std::move(s))
	{
	}

static void launch(std::unique_ptr<Socket> socket_ptr)
	{
	Handler{std::move(*socket_ptr)}();
	}

void spawn_handler(Socket&& s)
{
	std::unique_ptr<Socket> socket_ptr(new Socket(std::move(s)));
	std::thread{launch,std::move(socket_ptr)}.detach();
}
