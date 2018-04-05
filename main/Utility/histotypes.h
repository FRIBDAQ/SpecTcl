/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/

#ifndef __HISTOTYPES_H
#define __HISTOTYPES_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef __CPP_IOSTREAM_H
#include <iostream>
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

#ifndef __CRT_MATH_H
#include <math.h>
#define __CRT_MATH_H
#endif

#ifndef DAQDATATYPES_H
#include <daqdatatypes.h>
#endif





typedef enum _Datatype_t {
  keByte,			// Data type is byte.
  keWord,			// Data type is word.
  keLong,			// Data type is a long.
  keFloat,			// Data type is single precision float.
  keDouble,			// Data type is double precision float.
  keUnknown_dt
} DataType_t;

inline std::ostream&
operator<<(std::ostream& out, DataType_t t)
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
inline std::istream&
operator>>(std::istream& in, DataType_t& t)
{
  std::string value;
  t = keUnknown_dt;
  in >> value;
  if(value == std::string("byte")) t =  keByte;
  if(value == std::string("word")) t = keWord;
  if(value == std::string("long")) t = keLong;
  if(value == std::string("float"))t =  keFloat;
  if(value == std::string("double"))t = keDouble;
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
   kgPeak1d,
   kgFitline,
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
  ke2Dm,
  keG2DD,
  keGSummary
} SpectrumType_t;


// I/O for spectrum types.

inline std::ostream& 
operator<<(std::ostream& out, SpectrumType_t t)
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
  case keG2DD:
    out << "gd";
    break;
  case keGSummary:
    out << "gs";
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

inline std::istream& 
operator>>(std::istream& in, SpectrumType_t& t)
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
    case 'd':
      t = keG2DD;
      break;
    case 's':
      t = keGSummary;
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
    break;
  case '?':
  default:
    t = keUnknown;
    break;
  }
  return in;
}


// Default buffer size:

static const UInt_t knDefaultBufferSize   = kn1K*8;

// Default histogram storage size

static const UInt_t knDefaultSpectrumSize = 16*kn1M;

#endif

