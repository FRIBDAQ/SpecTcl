 /*
** Facility:
**	UNIX/VMS/PSOS portability
** Abstract:
**	mtbforbind.c	- This file contains fortran bindings to
**			  the mtblock.c routines.
** Author:
**	Ron Fox
**	NSCL
**	Michigan State University
**	East Lansing, MI 48824-1321
**	January 17, 1992
** SCCS Information:
**	Library_Source 1/27/92 @(#)mtforbind.c	2.1
**
*/
static char *version="Library_Source 1/27/92 @(#)mtforbind.c	2.1 Library_Source Ron Fox NSCL";
#ifdef VMS
#include <descrip.h>
#include "vmsstring.h"
#endif
#ifdef unix
#include <string.h>
#include <memory.h>
#endif
#include "mtaccess.h"
#include <ctype.h>
#include <errno.h>

/*-----------------------------------------------------------------
**
**	The pages that follow contain the f77 bindings to the
**	block oriented tape routines.
**-----------------------------------------------------------------
*/

/*
**  INTEGER f77mtopen_(INTEGER unit INTEGER willwrite)
**/
#ifdef unix
int f77mtopen_
#else
int f77mtopen
#endif
#ifdef __STDC__
             (int *unit, int *willwrite)
#else
             (unit, willwrite)
int *unit, *willwrite;
#endif
{
	return mtopen(*unit, *willwrite);
}
/*
**  INTEGER f77mtclose_(INTEGER channel)
*/
#ifdef unix
int f77mtclose_
#else
int f77mtclose
#endif
#ifdef __STDC__
              (int *channel)
#else
              (channel)
int *channel;
#endif
{
  	return mtclose(*channel);
}
/*
**	INTEGER f77wtdrive_(INTEGER channel)
*/
#ifdef unix
int f77wtdrive_
#else
int f77wtdrive
#endif
#ifdef __STDC__
              (int *channel)
#else
              (channel)
int *channel;
#endif
{
	return wtdrive(*channel);
}
/*
**	SUBROUTINE f77mtclearerr_(INTEGER channel)
*/
#ifdef unix
void f77mtclearerr_
#else
void f77mtclearerr
#endif
#ifdef __STDC__
    (int *channel)
#else
    (channel)
int *channel;
#endif

{
	mtclearerr(*channel);

}
/*
**	integer f77mtwrite_(integer unit, byte block(), integer count)
*/
#ifdef unix
int f77mtwrite_
#else
int f77mtwrite
#endif
#ifdef __STDC__
    (int *unit, char *block, int *count)
#else
    (unit, block, count)
int *unit;
char *block;
int *count;
#endif
{
	return mtwrite(*unit, block, *count);
}
/*
**	integer f77mtload_(integer unit)
*/
#ifdef unix
int f77mtload_
#else
int f77mtload
#endif
#ifdef __STDC__
    (int *unit)
#else
    (unit)
int *unit;
#endif
{
	return mtload(*unit);
}
/*
**	f77mtunload_(integer unit)
*/
#ifdef unix
int f77mtunload_
#else
int f77mtunload
#endif
#ifdef __STDC__
    (int *unit)
#else
    (unit)
int *unit;
#endif
{
	return mtunload(*unit);
}
/*
**	integer f77mtrewind(integer unit)
*/
#ifdef unix
int f77mtrewind_
#else
int f77mtrewind
#endif
#ifdef __STDC__
	(int *unit)
#else
       (unit)
int *unit;
#endif
{
	return mtrewind(*unit);
}
/*
**	integer f77mtread_(integer unit, byte block(), integer count,
**			integer actgual)
*/
#ifdef unix
int f77mtread_
#else
int f77mtread
#endif
#ifdef __STDC__
	(int *unit, char *block, unsigned *count, unsigned *actual)
#else
        (unit, block, count, actual)
int *unit;
char *block;
unsigned *count;
unsigned *actual;
#endif

{
	return mtread(*unit, block, *count, actual);
}
/*
** INTEGER f77mtweof_(integer unit, integer count)
*/
#ifdef unix
int f77mtweof_
#else
int f77mtweof
#endif
#ifdef __STDC__
    (int *unit, unsigned *count)
#else
    (unit, count)
int *unit;
unsigned *count;
#endif
{
   return mtweof(*unit, *count);
}
/*
** INTEGER f77mtspacef_(integer unit, integer count)
*/
#ifdef unix
int f77mtspacef_
#else
int f77mtspacef
#endif
#ifdef __STDC__
	(int *unit, int *count)
#else
        (unit, count)
int *unit;
int *count;
#endif
{
   return mtspacef(*unit, *count);
}
/*
** INTEGER f77mtspacer_(integer unit, integer count)
*/
#ifdef unix
int f77mtspacer_
#else
int f77mtspacer
#endif
#ifdef __STDC__
	(int *unit, int *count)
#else
        (unit, count)
int *unit;
int *count;
#endif
{
   return mtspacer(*unit, *count);
}

/*
**  The pages below contain f77 bindings to the volume level tape I/O
**  routines.
**/

/*
**  INTEGER f77VOLREAD(INTEGER VOLUME, BYTE DATA, INTEGER COUNT)
**
**	Read a block of data from a volume.
*/
#ifdef unix
int f77volread_
#else
int f77volread
#endif
#ifdef __STDC__
	(volume **vol, char *data, unsigned *count, unsigned *actual)
#else
        (vol, data, count, actual)
volume **vol;
char *data;
unsigned *count;
unsigned *actual;
#endif
{
    return volread(*vol, data, *count, actual);
}
/*
**  INTEGER f77volopen_(INTEGER VOLUME, ITEMLIST ITEMS[])
**
*/
#ifdef unix
int f77volopen_
#else
int f77volopen
#endif
#ifdef __STDC__
	(volume **vol, file_item *items)
#else
         (vol, items)
volume **vol;
file_item *items;
#endif
{
    return volopen(*vol, items);
}
/*
**  INTEGER f77VOLWRITE(INTEGER VOLUME, BYTE DATA[], INTEGER COUNT)
**
*/
#ifdef unix
int f77volwrite_
#else
int f77volwrite
#endif
#ifdef __STDC__
	(volume **vol, char *data, int *count)
#else
        (vol, data, count)
volume **vol;
char *data;
int *count;
#endif
{
   return volwrite(*vol, data, *count);
}
/*
**  INTEGER f77VOLMKSAFE(INTEGER VOLUME)
**
*/
#ifdef unix
int f77volmksafe_
#else
int f77volmksafe
#endif
#ifdef __STDC__
	    (volume **vol)
#else
            (vol)
volume **vol;
#endif
{
	int volmksafe(volume*);
    return volmksafe(*vol);
}
/*
**  INTEGER f77VOLCLOSE(INTEGER VOLUME)
*/
#ifdef unix
int f77volclose_
#else
int f77volclose
#endif
#ifdef __STDC__
	    (volume **vol)
#else
            (vol)
volume **vol;
#endif
{
    return volclose(*vol);
}
/*
**  INTEGER f77VOLCREATE(INTEGER VOLUME, ITEMLIST ITEMS)
**
*/
#ifdef unix
int f77volcreate_
#else
int f77volcreate
#endif
#ifdef __STDC__
	    (volume **vol, file_item *items)
#else
            (vol, items)
volume **vol;
file_item *items;
#endif
{
    volcreate(*vol, items);
}
/*
**  INTEGER f77VOLDMOUNT(INTEGER VOLUME)
**
*/
#ifdef unix
int f77voldmount_
#else
int f77voldmount
#endif
#ifdef __STDC__
	(volume **vol)
#else
        (vol)
volume **vol;
#endif
{
    return voldmount(*vol);
}
/*
** INTEGER f77VOLMOUNT(INTEGER FILID, ITEMLIST ITEMS)
**
**  -- Returns a volume handle to be used in other volume level operations
**
*/
#ifdef unix
int f77volmount_
#else
int f77volmount
#endif
#ifdef __STDC__
	(int *fileid, volume_item *items)
#else
        (fileid, items)
int *fileid;
volume_item *items;
#endif
{
    return (long)volmount(*fileid, items);
}
#ifdef unix
/*
**  SUBROUTINE f77tocstring(CHARACTER*(*) string)
**  -- NOTE for c internal use, this is actually a function
**	which returns the location of the original string
**/
#ifdef __STDC__
char *f77tocstring_(char *string, int len)
#else
char *f77tocstring_(string, len)
char *string;
int len;
#endif
{
   char *s;

   s = &string[len-1];		/* Point past end of string */
   while((s >= string) &&
         (isspace(*s) || (*s == '\0'))) {
      *s = '\0';
      s--;
   }
   return string;
}
#endif
/*
**  INTEGER v77VOLINIT(INTEGER FILEID, CHARACTER*(*) label)
**
*/
#ifdef unix
#ifdef __STDC__
int f77volinit_(int *fileid, char *label,int lbllen)
#else
int f77volinit_(fileid, label, lbllen)
int *fileid;
char *label;
int lbllen;
#endif
#endif
#ifdef VMS
int f77volinit(int *fileid, vmsstring *label)
#endif
{
#ifdef unix
  char lbl[100];
   char *f77tocstring_(char* , int);
  memset(lbl, 0, 100);
  strncpy(lbl, label, lbllen < 100 ? lbllen : 100);
   return volinit(*fileid, f77tocstring_(label,lbllen));
#endif
#ifdef vms
    char lbl[100];
    int i;

    vmstostr(lbl, label);	    /* Convert string to vms format */
    for (i = strlen(lbl);  i >= 0;  i--) /* Trim trailing F77 whitespace */
    {
	if (isspace (lbl[i]))
	{
	    lbl[i] = '\0';
	}
	else
	{
	    break;
	}
    }    
    return volinit(*fileid, lbl);
 
#endif
}


/*
** f77mtgetmsg - binding to errror message getting routine.
**
** CHARACTER*80 FUNCTION f77MTGETMSG(CODE)
**
*/
#ifdef unix
#ifdef __STDC__
void f77mtgetmsg_(char *buf, int maxsiz, int *code)
#else
void f77mtgetmsg_(buf, maxsiz, code)
char *buf;
int maxsiz;
int *code;
#endif
#endif
#ifdef VMS
void f77mtgetmsg(vmsstring *buf, int *code)
#endif
{
#ifdef unix
   char *msgdata;

   msgdata = mtgetmsg(*code);
   strncpy(buf, msgdata, maxsiz);
#endif
#ifdef VMS
    strncpy (buf->dsc$a_pointer, mtgetmsg(*code), buf->dsc$w_length);
    return;
#endif
}
/*
**  geterrno - get system error number value.
**
** INTEGER FUNCTION GETERRNO()
**
*/
#ifdef unix
#ifdef __STDC__
int geterrno_(void)
#else
int geterrno_()
#endif
#else
int geterrno(void)
#endif
{
   return errno;
}
