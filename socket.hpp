#pragma once
#include "sockaddr.hpp"


class Socket {
	
	int fd;

public:
	Socket(int domain, int type, int protocol = 0);
	Socket(int fd);	// Construct a socket object from an already open socket
	Socket(Socket& s) = delete;	// Copying not allowed
	Socket(Socket&& s);	// Move constructor


	Socket& operator=(Socket& s) = delete;
	Socket& operator=(Socket&& s) = delete;

	~Socket();

	void bind(const SocketAddress& sa);
	void listen(int backlog);
	Socket accept(SocketAddress& sa);
	void connect(const SocketAddress& sa);

	int write(const void * buffer, ssize_t length);

	int getfd() const;
};
