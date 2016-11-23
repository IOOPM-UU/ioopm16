#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

bool isnumber(const char *str)
{
  do
    {
      if (isdigit(*str) == false) return false;
    }
  while (*++str);

  return true;
}

bool parse_int_option(const char *opt, const char *arg, int *value)
{
  int opt_len = strlen(opt);
  if (strncmp(opt, arg, opt_len) == 0 && isnumber(arg+opt_len))
    {
      *value = atoi(arg+opt_len);
      return true;
    }
  return false;
}

