/*
** histotypes.h:
**    This file defines data types which are used by the 
**    histogramming package.
**
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   mailto: fox@nscl.msu.edu
**
** (c) Copyright 1999 NSCL, All rights reserved.
*/

#ifndef __HISTOTYPES_H
#define __HISTOTYPES_H

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

typedef enum {
  keByte,			// Data type is byte.
  keWord,			// Data type is word.
  keLong,			// Data type is a long.
  keFloat,			// Data type is single precision float.
  keDouble			// Data type is double precision float.
} DataType_t;

typedef enum {			// Types of display program gates.
   kgCut1d,			// 1-d cut gate (upper lower limit)
   kgContour2d,		// 2-d closed countour
   kgBand2d,			// 2-d open band
   kgGammaCut1d,
   kgGammaBand1d,
   kgGammaContour1d,
   kgGammaCut2d,
   kgGammaBand2d,
   kgGammaContour2d,
   kgUnSpecified		// Gate type not yet known.
} GateType_t;

typedef struct {		// A set of coordinates.
  Int_t nX;
  Int_t nY;
} Point_t;

typedef enum {			// types of spectra in prompter.
  keAny,			// Any type is legit.
  ke1d,				// 1-d only.
  ke2d,				// 2-d only.
  keCompatible			// Only those compatible with selected.
} DialogSpectrumType_t;

typedef enum {
  ke1D,
  ke2D,
  keBitmask,
  keSummary,
  keG1D,
  keG2D
} SpectrumType_t;

static const UInt_t kn1K                  = 1024;
static const UInt_t kn1M                  = kn1K*kn1K;

// Default buffer size:

static const UInt_t knDefaultBufferSize   = kn1K*8;

// Default histogram storage size

static const UInt_t knDefaultSpectrumSize = 16*kn1M;

#endif


