/*
** Facility:
**    Xamine file parses.
** Abstract:
**    lexsupport.c:
**      This file caters to the support needs of the newer flex/bison
**      processors which may be schizophrenic in their understanding of
**      C++
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/

#include <stdio.h>
int windfilewrap()
{
  return 1;
}
int grobjfilewrap()
{
  return 1;
}
int defaultfilewrap()
{
  return 1;
}

int yyval;
