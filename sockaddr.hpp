#pragma once
#include <sys/socket.h>

struct SocketAddress {
	sockaddr *address;
	socklen_t length;
};
