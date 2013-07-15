#ifndef INIT_LISTENER_H
#define INIT_LISTENER_H

/* Manages a socket set for listening. */
class Listener
{
	int sock_fd; // The file descriptor for the socket we're listening to
public:
	/* Initialize the socket, and return when it is bound to the correct
	 * address. backlog specifies how many connections will queue before
	 * they start getting refused.
	 */
	Listener(int backlog);
	
	/* Close the socket */
	~Listener();
	
	/* The file descriptor of the socket */
	int get_fd() const;
};

#endif