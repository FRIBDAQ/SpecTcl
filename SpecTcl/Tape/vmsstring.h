/* DEC/CMS REPLACEMENT HISTORY, Element VMSSTRING.H*/
/* *1    26-NOV-1991 11:56:21 FOX "Include file defining VMS String converts"*/
/* DEC/CMS REPLACEMENT HISTORY, Element VMSSTRING.H*/
/*  DEC/CMS REPLACEMENT HISTORY, Element VMSSTRING.H */
/*  *2    15-NOV-1990 08:39:12 FOX "Switch to C from C++" */
/*  *1    16-OCT-1990 11:14:59 FOX "C++ vms string class" */
/*  DEC/CMS REPLACEMENT HISTORY, Element VMSSTRING.H */
/*
**++
**  FACILITY:
**
**      C++ support.
**
**  ABSTRACT:
**
**      This file contains a definition for a VMSSTRING class.
**	The VMSSTRING handles VMS string descriptors.
**	What it does is generate string descriptors for existing C strings.
**	It is used in interfacing to VMS routines.
**
**  AUTHORS:
**
**      Ron Fox.
**
**
**  CREATION DATE:      Oct 10, 1990 
**	Include_file_VMS 1/27/92 @(#)vmsstring.h	2.1
**--
*/


/*
**
**  INCLUDE FILES
**
*/

#ifndef _VMSSTRING_
#define VMSSTRING
#include <descrip.h>

typedef struct {
		    unsigned short dsc$w_length;
		    unsigned char  dsc$b_dtype;
		    unsigned char  dsc$b_class;
		    char	   *dsc$a_pointer;
		} vmsstring;

extern  vmsstring *strtovms(vmsstring *dest, char *src);
extern  char      *vmstostr(char *dest, vmsstring *src);
extern  int       vmsstrlen(vmsstring *src);
#endif
