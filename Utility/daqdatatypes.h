/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


/*
**++
**  FACILITY:
**
**      Data types
**
**  ABSTRACT:
**
**      Data types for the host processor.
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

#ifndef __DAQTYPES_H

#include <sys/types.h>

typedef    char		    INT8;
typedef    unsigned char    UINT8;
typedef    short	    INT16;
typedef    unsigned short   UINT16;
typedef    int		    INT32;
typedef    unsigned int     UINT32;


#define SHORT_SIGNATURE 0x0102	/*	 
				**  Will be byte flipped by buffer swab
				*/	 

#define LONG_SIGNATUREHI 0x0102 /* Our assumption is that everything is  */
#define LONG_SIGNATURELO 0x0304  /* Written little endian */




typedef int Int_t;
typedef unsigned int UInt_t;

typedef short Short_t;
typedef unsigned short UShort_t;

typedef long Long_t;
typedef unsigned long ULong_t;

typedef float Float_t;
typedef double DFloat_t;



typedef char Char_t;
typedef unsigned char UChar_t;

typedef char (*Textsz_t);             // Null terminated string.
typedef UInt_t Size_t;

typedef void (*Address_t);           // Typical pointer.

static const Address_t kpNULL = (Address_t)0;  // Null pointer.

typedef enum {                    // State of a file.
	           kfsOpen,
		   kfsClosed
} FileState_t;

typedef enum {
  kacRead   = 1,		// Read access
  kacWrite  = 2,		// Write access
  kacCreate = 4,		// Create if needed.
  kacAppend = 8			// Append before writes.

} Access_t;

#define kACTIONSIZE 1024 // Size of action message.


typedef UChar_t Bool_t;
static const Bool_t kfTRUE = 0xff; // TRUE boolean.
static const Bool_t kfFALSE= 0;	  // FALSE boolean. 

typedef struct {
  UShort_t month,day, year;	//  date.
  UShort_t hours, min, sec;	// time in day.
} Time_t, *pTime_t;



static const UInt_t kn1K                  = 1024;
static const UInt_t kn1M                  = kn1K*kn1K;





#define __DAQTYPES_H
#endif
