#include "init_listener.hpp"
#include "configuration.hpp"
#include "logger.hpp"

#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <cerrno>

extern Log main_log;

static Socket openSocket()
	{

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
		try {
			Socket s(rp->ai_family,rp->ai_socktype,0); 
	
			main_log << DEBUG << "Created socket.\n";

			SocketAddress sa;
			sa.address = rp->ai_addr;
			sa.length = rp->ai_addrlen;
			try {
				s.bind(sa);
				main_log << DEBUG << "Bound socket onto address "
				         << rp->ai_addr->sa_data << "\n";
				freeaddrinfo(result); // Ideally this should be RAII
				return s;
				}
			catch (int &e) {
				main_log << DEBUG << "Attempt to bind to socket failed.\n";
				main_log << DEBUG << strerror(errno) << "\n";
				}
			}
		catch (int &e) {} // If an exception is thrown, we simply try again
		}
	freeaddrinfo(result);
	throw; //TODO: better exception
	}


Listener::Listener ( int backlog )
	: Socket(openSocket())
	{

	listen(backlog);

	main_log << INFO << "Initted listener on port " << get_config( "port" )
	         << "with fd " << getfd() << "\n";

	}
