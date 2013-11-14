#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <thread>

/* This starts a new thread to handle the request on the given file descriptor. */
void spawn_handler(int fd);

class Handler
	{
	int file_descriptor; //The socket this handler is attached to.
	size_t client_request_size;
	
	/* Send the buffer data to the socket fd.
	 * Return 0 on success, non-zero on failure.
	 */
	int send_data(const std::string &data);
	
public:
	Handler(int file_descriptor);
	/* Does the actual work of handling the client. This allows spawn_handler to
	* create a new thread and immediately return.
	*/
	void operator()();
	};
	
#endif
