/* CMS REPLACEMENT HISTORY, Element TYPES.H*/
/* *8     6-FEB-1996 09:11:05 FOX "Add Pointer types"*/
/* *7     5-FEB-1996 13:32:54 FOX "Fix pointer compilation errors"*/
/* *6    10-JAN-1996 13:10:54 FOX "Add *addr_t typedefs."*/
/* *5     4-JAN-1996 09:28:58 FOX "Add traditional type definitions"*/
/* *4    18-APR-1992 18:09:14 FOX "Add signature definitions"*/
/* *3     1-SEP-1989 16:23:47 FOX "Allow multiple inclusions"*/
/* *2    31-AUG-1989 17:44:56 FOX "Use CCONFIG logical rather than config"*/
/* *1     3-FEB-1988 11:35:38 FOX "Include file giving type for particular byte sizes"*/
/* CMS REPLACEMENT HISTORY, Element TYPES.H*/
#ifndef TYPES
/* DEC/CMS REPLACEMENT HISTORY, Element TYPES.H*/
/* *2     8-OCT-1987 09:10:12 FOX "Define TYPES id to indicate existence of types"*/
/* *1     7-OCT-1987 11:28:37 FOX "General data types."*/
/* DEC/CMS REPLACEMENT HISTORY, Element TYPES.H*/

/*
**++
**  FACILITY:
**
**      68000/68020 C compiler support.
**
**  ABSTRACT:
**
**      This module defines data types for the 68000/68020 processor
**
**  AUTHORS:
**
**      Ron Fox
**
**
**  CREATION DATE:     7-Oct-1987
**
**  MODIFICATION HISTORY:
**--
**/

typedef    char		    INT8;
typedef    unsigned char    UINT8;
typedef    short	    INT16;
typedef    unsigned short   UINT16;
typedef    int		    INT32;
typedef    unsigned int     UINT32;

/*
** Below are more traditional type definitions:
*/

typedef  char              byte;
typedef  unsigned char     ubyte;
typedef  short             word;
typedef  unsigned short    uword;
typedef  unsigned long     ulong;

#if OS != LINUX
typedef  byte *caddr_t;
#endif
typedef  word *waddr_t;
typedef  long *laddr_t;

#define SHORT_SIGNATURE 0x0102	/*	 
				**  Will be byte flipped by buffer swab
				*/	 

#define LONG_SIGNATUREHI 0x0102 /* Our assumption is that everything is  */
#define LONG_SIGNATURELO 0x0304  /* Written little endian */

#define TYPES							       /* 2 */
#endif
