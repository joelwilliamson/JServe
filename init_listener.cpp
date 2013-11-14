#include "init_listener.hpp"
#include "configuration.hpp"
#include "logger.hpp"

#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <cerrno>

extern Log main_log;

Listener::Listener ( int backlog ) {
	addrinfo hints, *result, *rp;

	/* The port to attempt binding to */
	int port = std::stoi ( get_config ( "port" ) );

	memset ( &hints,0,sizeof ( addrinfo ) );
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

	if ( getaddrinfo ( NULL,std::to_string ( port ).c_str(),&hints,&result ) == -1 ) {
		main_log << ERROR << "Error getting valid sockets. See "
		         << __FILE__ <<", line #" << __LINE__ << "\n";
		throw 10;
		}

	for ( rp = result; rp ; rp = rp->ai_next ) {
		if ( ( sock_fd = socket ( rp->ai_family,rp->ai_socktype,0 ) ) == -1 ) {
			main_log << DEBUG << "Attempt to open socket failed...\n";
			continue;
			}

		main_log << DEBUG << "Created socket.\n";

		if ( bind ( sock_fd, rp->ai_addr,rp->ai_addrlen ) == 0 ) {
			main_log << DEBUG << "Bound socket onto address "
			         << rp->ai_addr->sa_data << "\n";
			break;
			}
		else {
			main_log << DEBUG << "Attempt to bind to socket failed.\n";
			main_log << DEBUG << strerror(errno) << "\n";
			}

		close ( sock_fd ); /* The bind failed, so we need to release the
				 * file descriptor for reopening.
				 */

		}

	if ( listen ( sock_fd, backlog ) == -1 ) {
		main_log << ERROR << "Error listening on socket. See "
		         << __FILE__ << ", line #" << __LINE__ << "\n";
		main_log << ERROR << strerror(errno) << "\n";
		throw 10;
		}

	main_log << INFO << "Initted listener on port " << port
	         << "with fd " << sock_fd << "\n";

	freeaddrinfo ( result );
	}

Listener::~Listener() {
	close ( sock_fd );
	main_log << DEBUG << "Closed listening socket.\n";
	}

int Listener::get_fd() const {
	return sock_fd;
	}
