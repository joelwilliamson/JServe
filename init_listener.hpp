#ifndef INIT_LISTENER_H
#define INIT_LISTENER_H

#include "socket.hpp"

/* Manages a socket set for listening. */
class Listener : public Socket
{
public:
	/* Initialize the socket, and return when it is bound to the correct
	 * address. backlog specifies how many connections will queue before
	 * they start getting refused.
	 */
	Listener(int backlog);
};

#endif
