//  CNot.cpp
// Encapsulates a gate which produces the inverse of
// the single gate it contains (note that the single gate could
// be compound).
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//


#include "Not.h"                               
#include "SingleItemIterator.h"

static const char* Copyright = 
"CNot.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CNot

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t inGate ( CEvent& rEvent, vector<UInt_t>& Params )
//  Operation Type:
//     Evaluator.
//
Bool_t
CNot::inGate(CEvent& rEvent, vector<UInt_t>&)
// Evaluates the gate.  Returns the inverse
// of the value of the gate which is contained.
// 
// Formal Parameters:
//     CEvent& rEvent:
//        The event on which the gate is evaluated.
//     vector<UInt_t>& Params
//        the vector of parameters in the gate (empty)
//
// Exceptions: 
{
  return (!(m_pConstituent->operator()(rEvent)));
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator Begin (  )
//  Operation Type:
//     Selector.
//
CConstituentIterator 
CNot::Begin() 
{
// Produces an iterator which 'points' to the first
// gate in the set which is contained by this
// gate.  In this case, the iterator will 'point' to the
// gate contained by this.
//
// 

  // The iterator produced is a CConstituent iterator which cotnains
  // a pointer to a singleitem iterator.
  //
  return CConstituentIterator(*(new CSingleItemIterator));

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator End (  )
//  Operation Type:
//     Selector.
//
CConstituentIterator 
CNot::End() 
{
// Returns a special constituent iterator
// which 'points' to the end of the set of
// gates contained by this igate.
// Exceptions:  

  CConstituentIterator e(*(new CSingleItemIterator));
  ++e;
  return e;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t Size (  )
//  Operation Type:
//     Selector.
//
UInt_t 
CNot::Size() 
{
// Returns the number of constituents
// contained by this gate (1 in this case).
// Exceptions:  

  return 1;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string GetConstituent (  )
//  Operation Type:
//     Selector./dereferencer.
//
std::string 
CNot::GetConstituent(CConstituentIterator& rp) 
{
// Returns a textual equivalence for a consituent which 
// is pointed to by an iterator.
//
// In this case, returns the name of the gate
// which is pointed to by this gate.
//

  CSingleItemIterator* rI = (CSingleItemIterator*)rp.getActualIterator();
  if(rI->getNotEnd()) {		      // Not end of iteration...
    return m_pConstituent->getName(); //  so return name of gate.
  }
  else {
    return std::string("");
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CGate* clone (  )
//  Operation Type:
//     Duplicator...
//
CGate* 
CNot::clone() 
{
// Creates a clone of the current gate, allocated
// dynamically.

  return new CNot(*this);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string Type (  )
//  Operation Type:
//     selector
//
std::string 
CNot::Type() const
{
// Returns a type string indicating the type of
// gate this is.  In this case the string:
//    "-" is returned.
// Exceptions:  

  return std::string("-");
}
