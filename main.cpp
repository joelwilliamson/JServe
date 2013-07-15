#include "backend_manager.hpp"
#include "configuration.hpp"
#include "handler.hpp"
#include "init_listener.hpp"
#include "logger.hpp"

#include <memory>
#include <sys/socket.h>

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
	
	std::unique_ptr<Listener> listener;
	try { listener = std::unique_ptr<Listener>(new Listener(backlog));}
	catch (...)
	{
		exit(1);
	}
	
	for (int new_sock_fd; 1+(new_sock_fd = accept(listener->get_fd(),NULL,NULL));)
	{
		spawn_handler(new_sock_fd);
	}
	
}