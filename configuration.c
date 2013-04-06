#include "configuration.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char** config_list;

/*
 * All configuration options will be stored in a list similiar to argv.
 * The list will be successive key-value pairs. Eg char**l = {k1,l1,k2,l2,0};
 */
int read_config()
{
  int array_length = 4;
  int used = 0;
  config_list = malloc((array_length+1)*(sizeof(char*)));
  FILE* fd = fopen(CONFIG_FILE,"r");
  char line[256];
  char *key, *val;
  while(fgets(line,255,fd)) {
    if (line[0]=='#' || line[0] == '\n')
      continue;
    printf("Read line: %s\n",line);
    sscanf(line,"%ms = %ms",&key,&val);
    if (used==array_length) {
      /* Grow the array */
      array_length*=2;
      char** old_list = config_list;
      config_list = malloc((array_length+1)*sizeof(char*));
      memmove(config_list,old_list,used*sizeof(char*));
      free(old_list);
    }
    printf("key: %s\t",key);
    config_list[used++]=key;
    printf("val: %s\t",val);
    config_list[used++]=val;
  }
  config_list[used]=NULL;
  return 0;
}

char* get_config(char* var_name)
{
  int idx = 0;
  while (strcmp(var_name,config_list[idx])) idx+=2;
  return config_list[idx+1];
}
