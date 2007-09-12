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

static const char* Copyright = "(C) Copyright Michigan State University 2007, All rights reserved";
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

#include <config.h>
#include "GammaCut.h"
#include "SingleItemIterator.h"
#include <stdio.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
/*
  Change log:
  $Log$
  Revision 5.2.2.1  2007/05/30 19:37:09  ron-fox
  Backport new gate handling (exclusive of right limit) to 3.1
  from the 3.2 branch of the development.

  Revision 5.3  2007/05/30 15:51:40  ron-fox
  - Set 1d gates to be exclusive of the right side.
  - Fix up input so r.h. point is at right side of channel.
  - Fix up axis scaling.

  Revision 5.2  2005/06/03 15:19:20  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:22  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:03  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.4  2003/04/15 19:15:44  ron-fox
  To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs.

*/

// Functions for class CGammaCut

/*!
   Equality comparison operator.   Gamma gates compare equal if the
   base class comparison gives true and all constituents are the same
   on both sides of the ==.
   \param <TT>rhs (const CGammaCut& [in])</TT>
       The gate to which this will be compared.
   \retval
   - kfTRUE if the gates are equal.
   - kfFALSE if the gates are unequal.
*/
int
CGammaCut::operator==(const CGammaCut& rhs) const
{
  return (CGate::operator==(rhs)              &&
	  (m_nLow  == rhs.m_nLow)            &&
	  (m_nHigh == rhs.m_nHigh)           &&
	  (m_vSpecs== rhs.m_vSpecs));
}

/////////////////////////////////////////////////////////////////////
//
//  Function:
//    CGate* clone ()
//  Operation Type:
//    Construction
//  Purpose
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
    sprintf(Text, "%f %f", m_nLow, m_nHigh);
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


//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t inGate ( CEvent& rEvent, vector<UInt_t>& Params )
//  Operation Type:
//     Evaluator
//
Bool_t
CGammaCut::inGate(CEvent& rEvent, const vector<UInt_t>& Param)
{
  UInt_t Par = Param[0];
  if(Param[0] >= rEvent.size())
    return kfFALSE;
  else {
    if(rEvent[Par].isValid()) {
      Float_t nPoint = rEvent[Par];
      return((nPoint >= getLow()) && (nPoint < getHigh()));
    }
    else 
      return kfFALSE;
  }
}
