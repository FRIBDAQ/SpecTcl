//  CFalseGate.cpp
// A gate which is never made.
// This can be used to disable spectra.

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


#include "FalseGate.h"                               
#include "SingleItemIterator.h"
#include "ConstituentIterator.h"

#include <histotypes.h>

static const char* Copyright = 
"FalseGate.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CFalseGate

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t operator() ( CEvent& rEvent )
//  Operation Type:
//     functionalizer (gate check)
//
Bool_t 
CFalseGate::operator()(CEvent& rEvent) 
{
// Returns kfFALSE regardless of the event
//
// Formal Parameters:
//     CEvent& rEvent:
//            Event which is evaluated.
// Exceptions:  

  return kfFALSE;
}

Bool_t
CFalseGate::inGate(CEvent&, const vector<UInt_t>&)
{
  return kfFALSE;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator Begin (  )
//  Operation Type:
//     Selector
//
CConstituentIterator 
CFalseGate::Begin() 
{
// Returns a constituent iterator.  Since
// this gate type has no constituents, this
// function delegates to End()
// Exceptions:  

  return End();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator End (  )
//  Operation Type:
//     Selector.
//
CConstituentIterator 
CFalseGate::End() 
{
// Returns a constituent iterator which points to
// the end of the constituent list.

  // In this case we construct a single item iterator at end and
  // return it tucked inside a ConstituentIterator.

  CSingleItemIterator e(kfFALSE);
  return CConstituentIterator(e); // Return it packaged in the constituet it.
  

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t Size (  )
//  Operation Type:
//     Selector.
//
UInt_t 
CFalseGate::Size() 
{
// Returns the number of constituents.
// Note that this is 0 for this gate.
//
// Exceptions:  

  return 0;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string GetConstituent ( CConstituentIterator& rIterator )
//  Operation Type:
//     Selector.
//
std::string 
CFalseGate::GetConstituent(CConstituentIterator& rIterator) 
{
// Given a constituent iterator, returns
// the string representation of the constituent
// it points to.
//
// Formal Parameters:
//    CConstituentIterator& rIterator:
//       Iterator 'pointing' to the constituent.
// Returns:
//    In this case, an empty string is always returned.
// Exceptions:  

  std::string Constituent;
  return Constituent;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CGate* clone (  )
//  Operation Type:
//     virtual copy
//
CGate* 
CFalseGate::clone() 
{
// Creates a copy of this.
// Note that the copy is dynamically
// allocated and must be deleted.  This
//  is used where operator= might otherwise cause
// slicing.
// Exceptions:  

  return new CFalseGate(*this);     // Use the copy constructor.
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string Type (  )
//  Operation Type:
//     Selector.
//
std::string 
CFalseGate::Type() const
{
// Returns the type of gate which this is.
// This is the value "F"
// Exceptions:  

  return std::string("F");
}

