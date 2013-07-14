#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>

struct Request
{
	/* Reads a client request from the specified file descriptor and uses
	 * this data to create a formatted request.
	 */
	Request(int file_descriptor); 
	std::string method; // The HTTP method (ie GET, POST, etc.) used.
	std::string URI; // What did the client ask for?
	std::string http_version; // Typically 'HTTP/1.1'
	std::string raw; // The raw request
private:
	size_t client_request_size;
};

#endif