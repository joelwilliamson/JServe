/* The configuration file must be a series of lines, each either beginning with
 * a '#' or an empty line, or of the form "key = value", with key and value
 * having no whitespace characters and at least one whitespace character
 * separating each from the '='.
 */

/* read_config reads the various configuration files and sets the appropriate
 * global variables. The configuration variables can then be read with
 * get_config and changed with set_config.*/
#include <string>
int read_config();

/* get_config returns the contents of the variable named var_name. The pointer
 * targets the entry in config's internal data structure, so it shouldn't be
 * freed or modified.
 */
const std::string get_config(const std::string &var_name);

void delete_config();