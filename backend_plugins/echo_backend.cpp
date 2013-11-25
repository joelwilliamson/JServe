#include "plugin_types.h"
#include "../logger.hpp"

/*
 * echo_plugin sends an HTML file with the requested URI in the body and title
 */


extern Log main_log;


std::string provide_resource (const std::string &path, struct client_t* client, int &error_code)
	{
	std::string response(
		"<html>\r\n"
		"<head>\r\n\t<title>"
		+ path +
		"</title>\r\n</head>\r\n"
		"<body>\r\n<h2>"
		+ path +
		"</h2>\r\n</body>\r\n</html>" );
	return response;
	}

extern "C" std::list<backend_t> register_backend()
	{
	return std::list<backend_t>{{".echo",5,provide_resource,NULL}};
	}
