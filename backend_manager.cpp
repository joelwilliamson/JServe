#include "backend_manager.hpp"
#include "plugin_types.h"
#include "configuration.h"
#include "logger.hpp"

#include <dirent.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <limits.h>

#include <list>

extern Log main_log;

static std::list<backend_t> plugin_list;

/* Returns a pointer to the entry that is currently used for the given extension,
 * of NULL if no backend is yet registered.
 */
backend_t* extension_in_list(std::list<backend_t*> list, const std::string &ext)
{
	backend_t *best_so_far,dummy = {"",UINT_MAX,nullptr};
	best_so_far = &dummy;
	for (auto current : list) {
		if (current->str == ext || current->str == "de\fault") {
			if (current->priority <= best_so_far->priority) {
				best_so_far = current;
			}
		}
	}
	if (best_so_far != &dummy)
		return best_so_far;
	else
		return nullptr;
}

int register_backend_plugins()
{
	std::string backend_dir = get_config("backend_plugin_dir");
	
	main_log("Getting backend plugins from" + backend_dir, NOTICE);
	DIR* backends;
	struct dirent* next_file;
	std::string filepath;
	void* handle;
	char *dl_status_ptr;;
	std::string dl_status;
	
	if (!(backends=opendir(backend_dir.c_str()))) {
		int e_no = errno;
		main_log("Failed to open backend plugin directory " + backend_dir + ". Exiting...", ERROR);
		error(EXIT_FAILURE,e_no,"Opening directory failed");
	}
	
	errno = 0;
	while ((next_file = readdir(backends))) {
		filepath = backend_dir + "/" +  next_file->d_name;
		main_log("Considering " + filepath + " as a plugin.", DEBUG);
		if (filepath.substr(filepath.size()-3) != ".so") {
			continue;
		}
		main_log(filepath + " seems to be a plugin. Linking...");
		handle = dlopen(filepath.c_str(),RTLD_NOW);
		if (!handle) {
			main_log(std::string("Failed to link to plugin ") + next_file->d_name + ": " + dlerror(),ERROR);
			error(EXIT_FAILURE,0,"Linking plugin failed");
		} else {
			main_log("Opened plugin " + filepath + " for linking.", DEBUG);
		}
		
		std::list<backend_t> (*register_backend)();
		
		// Clear the dlerror
		dlerror();
		main_log("Cleared the error buffer. Trying to link...\n",INFO);
		register_backend = (register_backend_fn) (void*)dlsym(handle,"register_backend");
		main_log("Link complete. Checking status...", DEBUG);
		dl_status_ptr = dlerror();
		if (dl_status_ptr) dl_status = dl_status_ptr;
		main_log("Status is " + dl_status);
		if (dl_status.size()) {
			main_log(std::string("Failed to get symbol for 'register_backend': ") + dl_status, ERROR);
			error(EXIT_FAILURE,0,"Getting symbol failed.");
		} else {
			main_log("Got the 'register_backend' symbol.", DEBUG);
		}
		
		main_log("Attempting to register backends...", DEBUG);
		std::list<backend_t> result_list = register_backend();
		main_log("Got backend list.", DEBUG);
		
		/* Attempt to add each file extension returned to the handler list. */
		for (auto candidate : result_list) {
			for (auto current : plugin_list) {
				if (candidate.str == current.str && candidate.priority < current.priority) {
					current = candidate;
					main_log("Replaced handler with extension " + current.str + " to list.", DEBUG);
					goto success;
				}
			}
			plugin_list.push_back(candidate);
			main_log("Added handler with extension " + candidate.str + " to list.", DEBUG);
			success:;
		}
		main_log("Finished getting backends.",DEBUG);
	}
	closedir(backends);
	return 0;
}

std::string get_resource ( const std::string &path, struct client_t* client, int &error_code )
{
	main_log("Attempting to get response from " + path, DEBUG);
	std::string ext = strrchr(path.c_str(),'.');
	main_log("Using extension: " + ext,DEBUG);
	if (!ext.size()) ext.erase(1);
	else ext = "";
	
	backend_t handler;
	for (auto i : plugin_list) {
		if (ext == i.str) {
			handler = i;
			break;
		} else if ("de\fault" == i.str) {
			handler = i;
		}
	}
	main_log("Using handler with extension " + handler.str, DEBUG);
	
	return handler.provider(path,client,error_code);
}