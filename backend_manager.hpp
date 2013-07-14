#include <string>

struct client_t;

int register_backend_plugins();

/* Extract the file extension from path and use that to fetch the proper resource
 */
std::string get_resource(const std::string &path, client_t* client, int &error_code);

void delete_backend_list();