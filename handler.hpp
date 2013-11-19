#ifndef HANDLER_HPP
#define HANDLER_HPP


#include "socket.hpp"
#include <string>

/* This starts a new thread to handle the request on the given file descriptor. */
void spawn_handler( Socket&& s );

class Handler
	{
	Socket socket; //The socket this handler is attached to.
	
	/* Send the buffer data to the socket fd.
	 * Return 0 on success, non-zero on failure.
	 */
	int send_data( const std::string &data );
	int server_error( int err_code );
	
public:
	Handler( Socket&& s );
	/* Does the actual work of handling the client. This allows spawn_handler to
	* create a new thread and immediately return.
	*/
	void operator()();
	};
	
#endif
