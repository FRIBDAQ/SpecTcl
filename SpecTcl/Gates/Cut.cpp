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
//  CCut.cpp
// Encapsulates a cut on a parameter.
// The cut is described by a high and low limit 
// in parameter space.

//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

/*
  Change Log:
  $Log$
  Revision 5.1.2.3  2007/05/30 20:09:08  ron-fox
  Back port the new handling of gates from 3.2 into this branch

  Revision 5.2.2.1  2007/05/30 19:37:04  ron-fox
  Backport new gate handling (exclusive of right limit) to 3.1
  from the 3.2 branch of the development.

  Revision 5.3  2007/05/30 15:51:40  ron-fox
  - Set 1d gates to be exclusive of the right side.
  - Fix up input so r.h. point is at right side of channel.
  - Fix up axis scaling.

  Revision 5.2  2005/06/03 15:19:20  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.2  2005/05/27 17:47:36  ron-fox
  Re-do of Gamma gates also merged with Tim's prior changes with respect to
  glob patterns.  Gamma gates:
  - Now have true/false values and can therefore be applied to spectra or
    take part in compound gates.
  - Folds are added (fold command); and these perform the prior function
      of gamma gates.

  Revision 5.1.2.1  2004/12/21 17:51:21  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:02  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.7  2003/04/15 19:15:45  ron-fox
  To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs.

*/

//
// Header Files:
//

#include <config.h>
#include "Cut.h"                               
#include "SingleItemIterator.h"
#include <stdio.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Functions for class CCut

/*!
   Equality comparison operator.  This object is equal to the rhs
   iff the base class operator== is true and all member items
   are equal
*/
int
CCut::operator==(const CCut& aCut) const
{
  return (CGate::operator==(aCut)          &
	  (m_nLow   == aCut.m_nLow)        &
	  (m_nHigh  == aCut.m_nHigh)       &
	  (m_nId    == aCut.m_nId));
}
//
/*!
   Check the gate for an event.  The event makes the gate true
   iff the parameter associated with the event lies inside the
   limits.  No scaling  is done, as the gate end points are 
   considered to be in parameter space.
   \param <TT>rEvent  (CEvent& [in]) </TT>
      The event to check.  The parameter to check is in 
      m_nId.
   \retval Bool_t
    - kfTRUE if the gate was made.
    - kfFALSE if not.
*/
Bool_t
CCut::operator()(CEvent& rEvent)
{
  if(!wasChecked()) {
    Set(inGate(rEvent));
  }

  return getCachedValue();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t inGate ( CEvent& rEvent, vector<UInt_t>& Params )
//  Operation Type:
//     Evaluator
//
Bool_t
CCut::inGate(CEvent& rEvent, const vector<UInt_t>& Params)
// Returns the evaluation of the gate on this
// an event.
//
// Formal Parameters:
//    CEvent& rEvent:
//      Event on which the gate will be evaluated.
//    vector<UInt_t>& Params
//      vector of parameters in the gate (empty)
{
  return inGate(rEvent);
}

//////////////////////////////////////////////////////////////////////////
//
// inGate: check if the event makes the gate.
//
Bool_t
CCut::inGate(CEvent& rEvent)
{
  UInt_t id = getId();
  if(id >= rEvent.size()) {
    return kfFALSE;
  }
  else {
    if(rEvent[id].isValid()) {;
      Float_t nPoint = rEvent[id];
      return inGate(nPoint);
    }
    else {
      return kfFALSE;
    }
  }
  return kfFALSE;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CGate* clone (  )
//  Operation Type:
//     Construction
//
CGate* 
CCut::clone() 
{
// Returns a pointer to a gate which is a copy of
// the current gate.

  return new CCut(*this);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator Begin (  )
//  Operation Type:
//     Selector
//
CConstituentIterator 
CCut::Begin() 
{
// Returns an iterator which represents the
// start of the constituent list.

  CSingleItemIterator It;
  return CConstituentIterator(It);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator& End (  )
//  Operation Type:
//     selector
//
CConstituentIterator
CCut::End() 
{
// Returns an iterator which represents the end of the 
// constituent list.
// Exceptions:  

  CConstituentIterator p = Begin();
  p++;
  return p;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t Size (  )
//  Operation Type:
//     Selector
//
UInt_t 
CCut::Size() 
{
// Returns the number of constituents (1)
// Exceptions:  

  return 1;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string GetConstituent (  )
//  Operation Type:
//     Selector.
//
std::string 
CCut::GetConstituent(CConstituentIterator& rIterator) 
{
// Returns a text encoded version of the 
// constituent pointed to' by the iterator.
// 	
	

  CConstituentIterator e = End();
  if((rIterator != e)) {
    char Text[100];
    sprintf(Text, "%d %f %f", m_nId, m_nLow, m_nHigh);
    return std::string(Text);
  }
  else {
    return std::string("");
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string Type (  )
//  Operation Type:
//     Selector.
//
std::string 
CCut::Type() const
{
// Returns the type of gate.  In this case
// a single character string "s" for slice.  Note that
// "c" is used by contour.
// Exceptions:  

  return std::string("s");
}
//
Bool_t 
CCut::inGate(Float_t x)
{
  return ((x >= getLow()) && (x < getHigh()));
}
