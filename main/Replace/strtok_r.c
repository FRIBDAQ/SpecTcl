/* Portable version of strtok_r().
   This function is in the public domain. */

/*

@deftypefn Supplemental char* strtok_r (const char *@var{s}, int @var{c})

Returns a pointer to the last occurrence of the character @var{c} in
the string @var{s}, or @code{NULL} if not found.  If @var{c} is itself the
null character, the results are undefined.

@end deftypefn

*/

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <ansidecl.h>

char* strtok_r(char *s1, const char* s2, char** savept)
{
  if(s1) {
    *savept = s1;
  }
  return strsep(savept, s2);
}
