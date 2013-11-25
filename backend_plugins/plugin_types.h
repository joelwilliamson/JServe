#include <string>
#include <list>

struct client_t;


typedef std::string (*provider_fn)(const std::string &path, struct client_t* client, int& error_code);

struct backend_t {
	std::string str; // The file extension to handle
	unsigned int priority; // A lower number indicates a higher priority
	provider_fn provider; // The function that handles the actual work
	void* handle; // The dlopen() handle that was used to get this plugin
};

struct client_t {
	char* user_agent;
};

typedef std::list<backend_t> (*register_backend_fn)();
