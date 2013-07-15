#include "configuration.hpp" 
#include "logger.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <map>
#include <fstream>
#include <istream>
#include <limits>

static std::map<std::string, std::string> config_list;

extern Log main_log;

/*
 * All configuration options will be stored in a list similiar to argv.
 * The list will be successive key-value pairs. Eg char**l = {k1,l1,k2,l2,0};
 */
int read_config()
{
	std::ifstream config(CONFIG_FILE);
	main_log << INFO << "Opened file " << CONFIG_FILE << " for configuration.\n";
	std::string key, value;
	std::string seperator;
	while (config) {
		while (config.peek() == '#') config.ignore(std::numeric_limits< int >::max(),'\n');
		config >> key >> seperator >> value;
		main_log << DEBUG << key << " = " << value << "\n";
		config_list[key] = value;
	}
	
	return 0;
}

const std::string get_config(const std::string &var_name)
{
	return config_list[var_name];
}