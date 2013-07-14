#include "request.hpp"

#include "logger.hpp"

#include <unistd.h>

#include <sstream>

extern Log main_log;

Request::Request ( int file_descriptor )
	: client_request_size ( 1024 )
	{
	/* It is nescessary to use a char* rather than a std::string since read
	 * takes mutable C-strings as arguments.
	 */
	char* client_request = new char [client_request_size+1];
	size_t amount_read;

	/* Get the request from the socket */
	if ( ( amount_read = read ( file_descriptor,client_request,client_request_size ) ) == -1 )
		{
		// We enter this block if the read failed.
		throw 500; // Internal Server Error
		}
	else
		{
		client_request[amount_read] = 0; // This ensures we have a valid C-string
		}

	/* Parse the request header */
	std::stringstream request_stream ( client_request );
	request_stream >> method;
	request_stream >> URI;
	request_stream >> http_version;

	if ( request_stream.fail() )
		{
		main_log ( std::string ( "Received a bad request:\n" ) + client_request, NOTICE );
		throw 400; // Bad Request
		}
	else
		{
		main_log ( "Method: " + method + "\nRequested URI: " + URI + "\nHTTP Version: " + http_version, DEBUG );
		}

	raw = client_request;

	delete[] client_request;

	}