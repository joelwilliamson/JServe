/* A backend plugin handles the work of actually creating the Content-Body for
 * a response. A simple backend for HTML might just read the file from disk and
 * return it verbatim, while a more complex backend (eg. for .php) might
 * interpret a file and return the results. A backend doesn't have to read from
 * disk, it could get information from a DB, or return the same result regardless
 * of the requested resource.
 * 
 * 
 *            API
 * ======================================
 * On server startup, the backend plugin directory (specified in the conf file
 * with backend_dir = $backends) will be scanned for all files with a .so
 * extension. These are assumed to be plugins. The backend manager will link
 * with each of them, then call their register_backend() function.
 * 
 *    extern "C" std::list<backend_t> register_backend();
 * 
 * This function will return a list of backends. This is a list of structs, each
 * with string matching an extension the backend handles, a uint representing
 * the handler's priority, a pointer to the function to be used to provide the
 * resource and a pointer provided by dlopen(). A lower value for the priority
 * will cause the handler to be preferred over high values.
 * 
 * 	struct backend_t {
 *		std::string extension;
 *		unsigned int priority;
 *		provider_fn provider;
 *		void * handle;
 *	}
 *
 * If the plugin has the lowest priority of any that register with a given
 * extension, its provider() function will be called.
 * 
 *     typedef  std::string (*provider_fn) (const std::string &path, struct client_t* client, int& error_code);
 * 
 * This function will return a string that will be sent as is to the client.
 * The path will be $root_dir/$requested_URI, and client will contain
 * information on the client. If an error occurs, error_code will contain the
 * HTTP error (400,404,503, etc.).
 * 
 * Since each request is handled in a separate thread, it is necessary that
 * plugins be thread-safe.
 */
