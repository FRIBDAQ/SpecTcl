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
   Change log:
   $Log$
   Revision 5.4  2006/09/22 11:40:55  ron-fox
   - Cleaned up license text
   - Added support for m2 in SpectrumType_t enum and operator <<>>'s.

   Revision 5.3  2006/09/20 10:43:49  ron-fox
   Just some formatting (replacing licenses etc). stuff.

   Revision 5.2  2005/06/03 15:19:35  ron-fox
   Part of breaking off /merging branch to start 3.1 development

   Revision 5.1.2.3  2005/05/11 21:26:41  ron-fox
   - Add -pedantic and deal with the fallout.
   - Fix long standing issues with sread/swrite -format binary
   - Merge in Tim's strip chart spectrum and ensure stuff builds
     correctly.

   Revision 5.1.2.2  2005/05/11 16:56:48  thoagland
   Added Support for StripChart Spectra


   2005/05/05 Tim Hoagland
   Added support for StripChart Spectra
   

   Revision 5.1.2.1  2004/12/21 17:51:28  ron-fox
   Port to gcc 3.x compilers.

   Revision 5.1  2004/11/29 16:56:17  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 4.5  2003/08/25 16:25:33  ron-fox
   Initial starting point for merge with filtering -- this probably does not
   generate a goo spectcl build.

   Revision 4.4  2003/04/01 19:59:49  ron-fox
   Removed Mapped Spectrum types... Added changelog tag to header comments.

*/

#ifndef __HISTOTYPES_H
#define __HISTOTYPES_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef __CPP_IOSTREAM_H
#include <Iostream.h>
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

inline STD(ostream)&
operator<<(STD(ostream)& out, DataType_t t)
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
inline STD(istream)&
operator>>(STD(istream)& in, DataType_t& t)
{
  STD(string) value;
  t = keUnknown_dt;
  in >> value;
  if(value == STD(string)("byte")) t =  keByte;
  if(value == STD(string)("word")) t = keWord;
  if(value == STD(string)("long")) t = keLong;
  if(value == STD(string)("float"))t =  keFloat;
  if(value == STD(string)("double"))t = keDouble;
  return in;
}



typedef enum {			// Types of display program gates.
   kgCut1d,			// 1-d cut gate (upper lower limit)
   kgContour2d,	 	        // 2-d closed countour
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
  keG1D,
  keG2D,
  keUnknown,
  keStrip,
  ke2Dm
} SpectrumType_t;


// I/O for spectrum types.

inline STD(ostream)& 
operator<<(STD(ostream)& out, SpectrumType_t t)
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
  case keStrip:
    out << 'S';
    break;
  case keG2D:
    out << "g2";
    break;
  case ke2Dm:
    out << "m2";
    break;
  case keUnknown:
  default:
    out << '?';
    break;

  }
  return out;
}

inline STD(istream)& 
operator>>(STD(istream)& in, SpectrumType_t& t)
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
  case 'S':
    t = keStrip;
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
  case 'm':
    in >>c;
    if (c == '2') {
      t = ke2Dm;
    }
    else {
      t= keUnknown;
    }
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

#endif

