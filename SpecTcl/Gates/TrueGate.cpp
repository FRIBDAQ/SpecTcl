//  CTrueGate.cpp
// This class represents a gate which is always true.
//  In order to regularize spectrum/gate handling, spectra
//  are created with gates which are True.  The gate can
//  then be changed later.
//
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


#include "TrueGate.h"                               
#include "SingleItemIterator.h"

static const char* Copyright = 
"TrueGate.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CTrueGate

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t operator() ( CEvent& rEvent )
//  Operation Type:
//     Evaluator.
//
Bool_t 
CTrueGate::operator()(CEvent& rEvent) 
{
// Evaluates the gate.  In this case, kfTRUE is 
// returned regardless of the contents of the event.
// 
// Formal Parameters:
//      CEvent& rEvent:
//         Refers to the event.
// 
// Exceptions:  

  return kfTRUE;
  
}

Bool_t
CTrueGate::inGate(CEvent& rEvent, const vector<UInt_t>&)
{
  return kfTRUE;
}
Bool_t
CTrueGate::inGate(CEvent& rEvent)
{ 
  return kfTRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator Begin (  )
//  Operation Type:
//     Selector
//
CConstituentIterator 
CTrueGate::Begin() 
{
// Returns an iterator which points to the
// beginning of our constituent list.  Since
// we have no constituent list, this function
// returns End()
// Exceptions:  

  return End();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator End (  )
//  Operation Type:
//     Selector
//
CConstituentIterator 
CTrueGate::End() 
{
  CSingleItemIterator p;	// We construct a single item iteretor
  ++p;				// which is at end.
  return CConstituentIterator(p); // Return it bundled into the container.

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t Size (  )
//  Operation Type:
//     Selector.
//
UInt_t 
CTrueGate::Size() 
{
// Returns the number of entries in the constituent
// list.  In this case, 0 is returned.
// Exceptions:  

  return 0;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string GetConstituent ( CConstituentIterator& rIterator )
//  Operation Type:
//     Selector
//
std::string 
CTrueGate::GetConstituent(CConstituentIterator& rIterator) 
{
// Returns the text form of the constituent. 
// In this case, we return an empty string regardless
// of the iterator.
//
// Formal Parameters:
//    CConstituentIterator& rIterator:
//        Refers to the iterator which selects a
//        particular constituent.
// Exceptions:  

  return std::string("");
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CGate* clone (  )
//  Operation Type:
//     Virtual constructor.
//
CGate* 
CTrueGate::clone() 
{
// Returns a new copy of self.  The copy is dynamically created
// and must be explicitly deleted.
//
//
// Exceptions:  

  return new CTrueGate(*this);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string Type (  )
//  Operation Type:
//     Selector
//
std::string 
CTrueGate::Type() const
{
// Returns the gate type.  This is a string containing "T" for
// this case.
// Exceptions:  

  return std::string("T");
}
