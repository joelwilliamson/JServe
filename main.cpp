#include "backend_manager.hpp"
#include "configuration.hpp"
#include "handler.hpp"
#include "init_listener.hpp"
#include "logger.hpp"
#include "sockaddr.hpp"

#include <sys/socket.h>

#include <utility>	// std::move

int backlog = 100;
Log main_log ( "jserve.log" ,DEBUG);

int main() {

	try {
		read_config();
		}
	catch ( ... ) {
		main_log << ERROR << "Failed to read config. Dying...";
		}

	try {
		register_backend_plugins();
		}
	catch ( ... ) {
		main_log << ERROR << "Failed to register backend plugins. Dying...";
		exit ( 1 );
		}
	
	try {
		Listener listener(backlog);
		while (true) {
			SocketAddress sa;
			sa.address = NULL;
			Socket s(listener.accept(sa));
			spawn_handler(std::move(s));
			}
		}
	catch (...)
	{
		exit(1);
		}
	
}
