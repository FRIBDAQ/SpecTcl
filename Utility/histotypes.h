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

#ifndef __CPP_IOSTREAM_H
#include <iostream.h>
#define __CPP_IOSTREAM_H
#endif

#ifndef __CPP_STRING_H
#include <string.h>
#define __CPP_STRING_H
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

using namespace std;

#include <math.h>



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

typedef enum _Datatype_t {
  keByte,			// Data type is byte.
  keWord,			// Data type is word.
  keLong,			// Data type is a long.
  keFloat,			// Data type is single precision float.
  keDouble,			// Data type is double precision float.
  keUnknown_dt
} DataType_t;

inline ostream&
operator<<(ostream& out, DataType_t t)
{
  switch(t) {
  case keByte:
    out << "byte ";
    break;
  case keWord:
    out << "word ";
    break;
  case keLong:
    out << "long ";
    break;
  case keFloat:
    out << "float ";
    break;
  case keDouble:
    out << "double ";
    break;
  case keUnknown_dt:
  default:
    out << "???";
    break;
  }
  return out;
}
inline istream&
operator>>(istream& in, DataType_t& t)
{
  string value;
  t = keUnknown_dt;
  in >> value;
  if(value == string("byte")) t =  keByte;
  if(value == string("word")) t = keWord;
  if(value == string("long")) t = keLong;
  if(value == string("float"))t =  keFloat;
  if(value == string("double"))t = keDouble;
  return in;
}



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

typedef enum _SpectrumType_t {
  ke1D,
  ke2D,
  keBitmask,
  keSummary,
  keUnknown,
  keG1D,
  keG2D
} SpectrumType_t;


// I/O for spectrum types.

inline ostream& 
operator<<(ostream& out, SpectrumType_t t)
{
  switch(t) {
  case ke1D:
    out << '1';
    break;
  case ke2D:
    out << '2';
    break;
  case keBitmask:
    out << 'b';
    break;
  case keSummary:
    out << 's';
    break;
  case keG1D:
    out << "g1";
    break;
  case keG2D:
    out << "g2";
    break;
  case keUnknown:
  default:
    out << '?';
    break;

  }
  return out;
}

inline istream& 
operator>>(istream& in, SpectrumType_t& t)
{
  char c;
  in >> c;
  switch(c) {
  case '1':
    t = ke1D;
    break;
  case '2':
    t = ke2D;
    break;
  case 'b':
    t = keBitmask;
    break;
  case 's':
    t = keSummary;
    break;
  case 'g':
    in >> c;
    switch(c) {
    case '1':
      t = keG1D;
      break;
    case '2':
      t = keG2D;
      break;
    default:
      t = keUnknown;
      break;
    }
    break;
  case '?':
  default:
    t = keUnknown;
    break;
  }
  return in;
}

static const UInt_t kn1K                  = 1024;
static const UInt_t kn1M                  = kn1K*kn1K;

// Default buffer size:

static const UInt_t knDefaultBufferSize   = kn1K*8;

// Default histogram storage size

static const UInt_t knDefaultSpectrumSize = 16*kn1M;


// stands in for log2f function which is not included with linux c++
#ifdef Linux
inline float log2f(float a) {
  return (float)(log(a)/log(2));
}
#endif

#endif

