/* DEC/CMS REPLACEMENT HISTORY, Element VMSSTRING.C*/
/* *1    26-NOV-1991 11:56:38 FOX "VMS string conversion routines"*/
/* DEC/CMS REPLACEMENT HISTORY, Element VMSSTRING.C*/
/* DEC/CMS REPLACEMENT HISTORY, Element VMSSTRING.C*/
/* *1    15-NOV-1990 08:41:28 FOX "C VMS<->string conversions"*/
/* DEC/CMS REPLACEMENT HISTORY, Element VMSSTRING.C*/

/*
**++
**  FACILITY:
**
**      C support.
**
**  ABSTRACT:
**
**      vmsstring.c - This file contains a couple of routines useful in 
**		    manipulating VMS string descriptors.
**
**  AUTHORS:
**
**      Ron Fox
**
**
**  CREATION DATE:      Nov 14, 1990
**
**  MODIFICATION HISTORY:
**	Library_source_VMS 1/27/92 @(#)vmsstring.c	2.1
**--
*/

static char *version = "Library_source_VMS 1/27/92 @(#)vmsstring.c	2.1 Ron Fox NSCL\n";
/*
**
**  INCLUDE FILES
**
*/

#include "vmsstring.h"



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      strtovms    - Fill in a string descriptor given a C string.
**		    NOTE: no storage is allocated.  The descriptor will
**			exactly describe the current state of the C string.
**
**  FORMAL PARAMETERS:
**
**      vmsstring *dest	- Destination string descriptor.
**	char	*src	- pointer to source string.
**
**
**  FUNCTION VALUE:
**
**      Returns a pointer to the vmsstring descriptor.
**
**
**--
*/
vmsstring  *strtovms (vmsstring *dest, char *src)
{

    dest->dsc$w_length = strlen (src);
    dest->dsc$a_pointer= src;
    return dest;
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      vmstostr    - Copies a VMS string to a C string.
**
**  FORMAL PARAMETERS:
**
**      char	*dest	- Pointer to destnation C string.
**	vmsstring *src  - POinter to source VMS string.
**
**  FUNCTION VALUE:
**
**      pointer to the destination C string.
**
**
**--
*/
char *vmstostr (char *dest, vmsstring *src)
{

    strncpy (dest, src->dsc$a_pointer, src->dsc$w_length+1);
    return dest;;
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      vmsstrlen   - Returns the length of a VMS string.
**
**  FORMAL PARAMETERS:
**
**      vmsstring *src
**
**
**  FUNCTION VALUE:
**
**      dsc$w_length.
**
**--
*/
int vmsstrlen (vmsstring *src)
{

    return src->dsc$w_length;
}
