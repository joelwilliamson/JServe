#include <sys/socket.h>

class SocketAddress {
public:
	sockaddr address;
	socklen_t length;
};
