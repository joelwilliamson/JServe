#include "plugin_types.h"
#include <stdlib.h>
#include <list>

const std::list<backend_t> register_backend()
{
	struct backend_t data;
	data.priority=100;
	data.str="";
	return std::list<backend_t>(1,data);
}