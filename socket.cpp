#include "socket.hpp"

#include <sys/socket.h>
#include <netinet/in.h>	// Internet Protocol
#include <sys/un.h>	// Unix sockets
#include <arpa/inet.h>	// Manipulating IP addresses
#include <unistd.h>	// close()


#include <cerrno>
#include <cstring>
#include <algorithm>

#include "logger.hpp"

extern Log main_log;

Socket::Socket(int domain, int type, int protocol)
	: fd(socket(domain,type,protocol))
	{
	if (fd == -1) {	// The socket failed to open
		main_log << ERROR << "Socket failed to open: " << strerror(errno) << '\n';
		throw errno; // TODO: better exception
		}
	}

Socket::Socket(int fd)
	: fd(fd)
	{}

Socket::Socket(Socket&& s)
	: fd(0)
	{
	std::swap(fd,s.fd);
	}

Socket::~Socket()
	{
	if (fd)	while (close(fd) && errno != EBADF); // If the file descriptor is valid,
			// try to close it
	}

void Socket::bind(const SocketAddress& sa)
	{
	if (::bind(fd,sa.address,sa.length)) {
		main_log << ERROR << "Socket failed to bind: " << strerror(errno) << '\n';
		throw errno; // TODO: better exception
		}
	}

void Socket::listen(int backlog)
	{
	if (::listen(fd,backlog)) {
		main_log << ERROR << "Failed to listen on socket: " << strerror(errno) << '\n';
		throw errno; // TODO: better exception
		}
	}

Socket Socket::accept(SocketAddress& sa)
	{
	int new_fd = ::accept(fd,sa.address,&sa.length);
	if (new_fd == -1) {
		main_log << ERROR << "Failed to accept on socket: " << strerror(errno) << '\n';
		throw errno; // TODO: etc
		}
	return Socket(new_fd);
	}

void Socket::connect(const SocketAddress& sa)
	{
	if (::connect(fd,sa.address,sa.length)) {
		main_log << ERROR << "Failed to connect to socket: " << strerror(errno) << '\n';
		throw errno; // TODO: etc
		}
	}

int Socket::getfd() const
	{
	return fd;
	}

int Socket::write(const void * buffer, ssize_t length)
	{
	return ::write(fd,buffer,length);
	}
