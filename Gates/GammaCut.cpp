// CGammaCut.cpp
// Encapsulates a gamma cut on a spectrum
// The cut is described by a high and low limit
//
//  Author:
//     Jason Venema
//     NSCL
//     Michigan State University
//     East Lansing, MI  48824-1321
//     mailto:venemaja@msu.edu
//
//////////////////////////////////////////////////////////////////////////

#include "GammaCut.h"
#include "SingleItemIterator.h"
#include <stdio.h>

static const char* Copyright =
"CGammaCut.cpp: Copyright 2001 NSCL, All Rights Reserved\n";

// Functions for class CGammaCut

/////////////////////////////////////////////////////////////////////
//
//  Function:
//    CGate* clone ()
//  Operation Type:
//    Construction
//  Purpose:
//    Returns a pointer to a gate which is a 
//    copy of the current gate.
//
CGate*
CGammaCut::clone()
{
  return new CGammaCut(*this);
}

//////////////////////////////////////////////////////////////////////
//
//  Function:
//    std::string GetConstituent()
//  Operation Type:
//    Selector
//  Purpose:
//    Returns a text encoded version of the
//    constituent 'pointed to' by the iterator.
//
std::string
CGammaCut::GetConstituent (CConstituentIterator& rIterator)
{
  CConstituentIterator e = End();
  if (rIterator != e) {
    char Text[100];
    sprintf(Text, "%d %d", m_nLow, m_nHigh);
    return std::string(Text);
  }
  else {
    return std::string("");
  }
}

/////////////////////////////////////////////////////////////////////
//
//  Function:
//    std::string Type ()
//  Operation Type:
//    Selector
//  Purpose:
//    Returns the type of gate. In this case,
//    a two character string "gs" for gamma slice.
//    Note that "gc" is used by gamma contour
//
std::string
CGammaCut::Type() const
{
  return std::string("gs");
}
