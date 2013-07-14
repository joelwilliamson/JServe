/* This backend will have a priority of UINT_MAX so any other plugin will be
 * preferred over it. It acts in the simplest possible manner: it tries to read
 * the requested file and returns exactly that. It will accept any extension
 * by registering the special keyword "de\fault". To register a different
 * default handler, use this keyword with a lower priority.
 */

#include "plugin_types.h"
#include "../logger.hpp"

#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <string>
#include <list>

extern Log main_log;

backend_t default_backend;

std::string provide_resource(const std::string &path, struct client_t* client, int &error_code)
{
	struct stat statbuf;
	if (stat(path.c_str(),&statbuf)==-1) {
		main_log("Couldn't find file " + path + " " + strerror(errno));
		error_code = 404;
		return nullptr;
	}
	off_t file_size = statbuf.st_size;
  
	/* Create the Response */
	char *response_body = new char [file_size + 1];
	int rf;
	if ((rf = open(path.c_str(),O_RDONLY)) == -1) {
		main_log("File " + path + " failed to open by plugin " __FILE__ + ": " + strerror(errno) + "\n");
		error_code = 503;
		return nullptr;
	}
  
	int bytes_read;
	if ((bytes_read = read(rf,response_body,file_size)) == -1) {
		main_log("Error reading file " + path + " in plugin " + __FILE__ + ": " + strerror(errno) + "\n");
		error_code = 503;
		return nullptr;
	} else {
		response_body[bytes_read]='\0';
	}
  
	return response_body;
}

extern "C" const std::list<backend_t> register_backend()
{
	default_backend.str = "de\fault";
	default_backend.priority = UINT_MAX;
	default_backend.provider = provide_resource;
	return std::list<backend_t>(1,default_backend);
}