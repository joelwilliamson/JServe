#include "configuration.h"
#include "logger.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char** config_list;

extern Log main_log;

/*
 * All configuration options will be stored in a list similiar to argv.
 * The list will be successive key-value pairs. Eg char**l = {k1,l1,k2,l2,0};
 */
int read_config()
{
  int array_length = 4;
  int used = 0;
  config_list = new char* [array_length+1];
  FILE* fd = fopen(CONFIG_FILE,"r");
  char line[256];
  char *key, *val;
  while(fgets(line,255,fd)) {
    if (line[0]=='#' || line[0] == '\n')
      continue;
    main_log(std::string("Read line: ") + line,DEBUG);
    sscanf(line,"%ms = %ms\n",&key,&val);
    if (used==array_length) {
      /* Grow the array */
      array_length*=2;
      char** old_list = config_list;
      config_list = new char* [array_length+1];
      memmove(config_list,old_list,used*sizeof(char*));
      free(old_list);
    }
    main_log(std::string("key: ") + key);
    config_list[used++]=key;
    main_log(std::string("val: ") +val);
    config_list[used++]=val;
  }
  config_list[used]=NULL;
  fclose(fd);
  return 0;
}

const std::string get_config(const std::string &var_name)
{
  int idx = 0;
  while (strcmp(var_name.c_str(),config_list[idx])) idx+=2;
  return std::string(config_list[idx+1]);
}

void delete_config()
{
	char **next = config_list;
	do {
		main_log(std::string("Freeing string ") + *next + " at " + std::to_string((long long int)next));
		free(*next);
		next++;
	} while (*next);
	free(config_list);
}
