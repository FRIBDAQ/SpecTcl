/*
** Facility:
**   Display program graphical objects.
** Abstract:
**   grobfile.cc  - This file contains code to handle I/O of graphical objects
**                  from a file.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
char *sccsversion="@(#)grobfile.cc	8.1 6/23/95 ";
/*
** Include files:
*/
#include <stdio.h>
#ifdef unix
#include <sys/types.h>
#endif
#ifdef VMS
#include <types.h>
#endif
#include <time.h>
#include "dispgrob.h"
#include "panemgr.h"
#include "errormsg.h"
FILE *yyin = 0;			/* Define here to allow */
FILE *yyout = 0;		/* multipler lexers.    */
int grobj_database::m_ReadOnce = 0;    // Need to reset lexer.
void grobjfilerestart(FILE *fp);

/*
** Method Description:
**   grobj_generic::write   - This method simply writes out the points list
**                            for a graphical object.
** Formal Parameters:
**   FILE *f    - File pointer open on the output file.
**
** Returns:
**   Number of bytes written to file or EOF if failed.
*/
int grobj_generic::write(FILE *f)
{
  int nbytes = 0;		/*  Total bytes written */
  int bytes;			/* Bytes written this I/o */


  bytes = fprintf(f, "   spectrum %d\n", spectrum);
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  

  /* Write the object's name if it exists. */

  if(hasname) {
    bytes = fprintf(f,"   name \"%s\"\n", name);
    if(bytes == EOF) return EOF;
    nbytes += bytes;
  }

  if(npts > 0) {

    /* WRite the header */

    bytes = fprintf(f, "   beginpts\n");
    if(bytes == EOF) return EOF;
    nbytes += bytes;

    /* Write the points: */

    for(int i = 0; i < npts; i++) {
      bytes = fprintf(f, "      %d,%d\n", pts[i].getx(), pts[i].gety());
      if(bytes == EOF) return EOF;
      nbytes += bytes;
    }

    bytes = fprintf(f, "   endpts\n");
    if(bytes == EOF) return EOF;
    nbytes += bytes;
  }
  return nbytes;

}

/*
**  Method descriptions:
**    grobj_xxxx:write  - These methods put the appropriate OBJECT/ENDOJBECT
**                        encapsulation around a generic write of an object.
**                        The object type used depends on the actual
**                        underlying object type being written.
** Formal Parameters:
**   FILE *f  - File pointer open on a write stream file
** Returns:
**   EOF if failed.
**   Count of bytes written otherwise.
*/

int grobj_cut1d::write(FILE *f)
{
  int nbytes = 0;
  int bytes;

  /* Write the object header... */

  bytes = fprintf(f, "object cut\n");
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the guts of the object: */

  bytes = grobj_generic::write(f);
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the trailer... */

  bytes = fprintf(f, "endobject\n");
  if(bytes == EOF) return EOF;
  return (nbytes += bytes);
}

int grobj_sum1d::write(FILE *f)
{
  int nbytes = 0;
  int bytes;

  /* Write the object header... */

  bytes = fprintf(f, "object sum-1d\n");
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the guts of the object: */

  bytes = grobj_generic::write(f);
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the trailer... */

  bytes = fprintf(f, "endobject\n");
  if(bytes == EOF) return EOF;
  return (nbytes += bytes);
}
int grobj_mark1d::write(FILE *f)
{
  int nbytes = 0;
  int bytes;

  /* Write the object header... */

  bytes = fprintf(f, "object mark-1d\n");
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the guts of the object: */

  bytes = grobj_generic::write(f);
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the trailer... */

  bytes = fprintf(f, "endobject\n");
  if(bytes == EOF) return EOF;
  return (nbytes += bytes);
}
int grobj_contour::write(FILE *f)
{
  int nbytes = 0;
  int bytes;

  /* Write the object header... */

  bytes = fprintf(f, "object contour\n");
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the guts of the object: */

  bytes = grobj_generic::write(f);
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the trailer... */

  bytes = fprintf(f, "endobject\n");
  if(bytes == EOF) return EOF;
  return (nbytes += bytes);
}
int grobj_band::write(FILE *f)
{
  int nbytes = 0;
  int bytes;

  /* Write the object header... */

  bytes = fprintf(f, "object band\n");
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the guts of the object: */

  bytes = grobj_generic::write(f);
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the trailer... */

  bytes = fprintf(f, "endobject\n");
  if(bytes == EOF) return EOF;
  return (nbytes += bytes);
}
int grobj_sum2d::write(FILE *f)
{
  int nbytes = 0;
  int bytes;

  /* Write the object header... */

  bytes = fprintf(f, "object sum-2d\n");
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the guts of the object: */

  bytes = grobj_generic::write(f);
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the trailer... */

  bytes = fprintf(f, "endobject\n");
  if(bytes == EOF) return EOF;
  return (nbytes += bytes);
}
int grobj_mark2d::write(FILE *f)
{
  int nbytes = 0;
  int bytes;

  /* Write the object header... */

  bytes = fprintf(f, "object mark-2d\n");
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the guts of the object: */

  bytes = grobj_generic::write(f);
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Write the trailer... */

  bytes = fprintf(f, "endobject\n");
  if(bytes == EOF) return EOF;
  return (nbytes += bytes);
}

/*
** Method description:
**   grobj_database::write   - This method writes an entire graphical object
**                             database to file.
**                             If a title is supplied, the file is prefaced
**                             with a title string. In any event,
**                             comments are generated which include the
**                             SCCS ID of the module and the date/time
**                             of generation.
** Formal Parameters:
**   FILE *f      - File stream pointer open on the output file.
** Returns:
**   Number of bytes written to file or
**   EOF if no bytes were written.
*/

int grobj_database::write(FILE *f)
{
  int nbytes = 0;
  int bytes;
  time_t t;

  /* Write the database title/name string if it exists. */

  if(hasname) {
    bytes = fprintf(f, "title \"%s\"\n", dbname);
    if(bytes == EOF) return EOF;
    nbytes += bytes;
  }

  /* Write comments which indicate who wrote the file and when */

  time(&t);			/* Get the time in secs since epoch */
  bytes = fprintf(f, "\n#  Graphical object file created by %s on %s \n\n",
		  sccsversion, ctime(&t));
  if(bytes == EOF) return EOF;
  nbytes += bytes;

  /* Now write out the objects: */

  for(int i = 0; i < obj_count; i++) {
    bytes = objects[i]->write(f);
    if(bytes == EOF) return EOF;
    nbytes += bytes;
  }

  /* Return the size of the file: */

  return nbytes;
}

/*
** Method Description:
**   grobj_database::read  - This function reads in a database from
**                           file using the bison/yacc flex/lex generated
**                           parser.
**
** Formal Parameters:
**    FILE *f   - File pointer for the stream open on the file we'll read
**                from.
** Returns:
**    False Failure.
**    True  Success..
*/
grobj_database *grobj_db;
extern FILE *grobjfilein;
extern void grobj_startlex();
int grobjfileparse();
int grobj_database::read(FILE *f)
{
  int status;

  grobj_db = this;		/* Set the global object pointer. */
  if(m_ReadOnce)grobjfilerestart(f);  // Need to reset buffering.
  m_ReadOnce = -1;
  grobjfilein = f;		/*  Set the source for the read. */
  status = grobjfileparse();
  return status;

}
int grobfilewrap()
{
  return 1;
}
#ifdef FLEXV2
void grobj_startlex();
void grobjfilerestart(FILE *f)
{
  grobjfilein = f;
  grobj_startlex();
  
}
#endif

/*
** yyerror -- called when there is an error in the parse.
*/
extern int grob_lineno;
void grobjfileerror(char *str)
{
  char errormsg[256];


  sprintf(errormsg, "Line %d:, %s\n",
	  grob_lineno, str);
  Xamine_error_msg(Xamine_Getpanemgr(),
		   errormsg);
}
