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

#if __cplusplus
#if __cplusplus > 199711L // i.e. C++98
// stdint.h is deprecated as of C++11 in favor of <cstdint>
// The difference is that the types are brought into the std namespace.
// We can mimic the old behavior by including cstdint and then bringing
// them into the global scope with the using operator. This is a kludge 
// but is the best solution for the moment.
#include <cstdint>
using std::int8_t;
using std::uint8_t;
using std::int16_t;
using std::uint16_t;
using std::int32_t;
using std::uint32_t;
using std::int64_t;
using std::uint64_t;
#else
// When compiled with a C++ compiler that is not C++11 compatible
// cstdint does not exist. Furthermore, stdint.h only includes the 
// limit macros if __STDC_LIMIT_MACROS variables is defined. 
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#endif /* __cplusplus */
#else
// macros are part of the c standard
# include <stdint.h>
#endif /* C++ or C */

typedef    int8_t		   INT8;
typedef    uint8_t     UINT8;
typedef    int16_t	   INT16;
typedef    uint16_t    UINT16;
typedef    int32_t	   INT32;
typedef    uint32_t    UINT32;



#define SHORT_SIGNATURE 0x0102	/*	 
				**  Will be byte flipped by buffer swab
				*/	 

#define LONG_SIGNATUREHI 0x0102 /* Our assumption is that everything is  */
#define LONG_SIGNATURELO 0x0304  /* Written little endian */




typedef int32_t  Int_t;
typedef uint32_t UInt_t;

typedef int16_t  Short_t;
typedef uint16_t UShort_t;

typedef int32_t  Long_t;
typedef uint32_t ULong_t;

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
