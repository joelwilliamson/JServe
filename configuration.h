/* The configuration file must be a series of lines, each either beginning with
 * a '#' or an empty line, or of the form "key = value", with key and value
 * having no whitespace characters and at least one whitespace character
 * separating each from the '='.
 */

/* read_config reads the various configuration files and sets the appropriate
 * global variables. The configuration variables can then be read with
 * get_config and changed with set_config.*/
int read_config();

/* get_config returns the contents of the variable named var_name. */
char* get_config(char* var_name);