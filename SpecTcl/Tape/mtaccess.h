/*
**++
**  FACILITY:
**
**      EXABYTE - NSCL exabyte tape support for front ends.
**
**  ABSTRACT:
**
**      MTACCESS.H  - This include file contains 
**		      definitions at the user level for constants, data
**		      structures, and function entry points associated with
**		      the mtaccess routines.
**
**  AUTHORS:
**
**      Ron Fox
**
**
**  CREATION DATE:      28-Feb-1990
**	Include_file G% @(#)mtaccess.h	2.1
**--
*/

#ifndef _MTACCESS_INSTALLED_
#define _MTACCESS_INSTALLED_
/*
**
**  INCLUDE FILES
**
*/

#include "mtinternl.h"

/*
**
**  Type definitions:
**
*/

//typedef char volume;	    /* Essentially opaque volume type */
//typedef int tape_ucb;	    /* Opaque type for UCB	    */

/* In VMS systems there can be name space conflicts between e.g. fortran
** and C routines, so we make the following substitutions for vms:
*/

/*
**
**  Function definitions (with prototypes)
**
*/
	    /* Block level I/O access */
#if defined( __STDC__) || defined(__cplusplus)
#ifdef __cplusplus
extern "C" {
#endif
int mtopen(int unit, int willwrite);

int mtclose(int unit);

int wtdrive(int unit);

int mtclearerr(int unit);

int mtwrite (int unit,
	    void *block, 
	    unsigned count);

int mtload(int unit);

int mtunload(int unit);

int mtrewind (int handle);

int mtread (int handle,  
	    char *block, 
	    unsigned count, unsigned *actual );

int mtweof (int handle,
	    unsigned count );

int mtspacef (int handle,  
	      int count );

int mtspacer (int handle,  
	      int count );

char *mtgetmsg(int code);

	/* Volume level I/O functions */

int volinit (int handle,  
		 char *label );

volume *volmount (
		  int unit, 
		  volume_item *itemlist);

int voldmount (volume *vol);

int volcreate (volume *vol, 
	       file_item *itemlist );

int volopen (volume *vol, 
	     file_item *itemlist);

int volwrite (volume *vol, 
	      char *data,
	      int count);

int volread (volume *vol, 
	     char *data,
	     int count,
	     int *actual);

int volclose (volume *vol );

#ifdef __cplusplus
}
#endif
#else

int mtopen();
int mtclose();
int wtdrive();
int mtclearerr();
int mtwrite();
int mtload();
int mtunload();
int mtrewind();
int mtread();
int mtweof();
int mtspacef();
int mtspacer();
char * mtgetmsg();
int volinit();
volume *volmount();
int voldmount();
int volopen();
int volwrite();
int volread();
int volclose();

#endif

#endif




