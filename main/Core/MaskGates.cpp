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
//  CTrueGate.cpp
// This class represents a group of gates that compares
//  a parameter to a mask value.  
//
//  Author:
//    Timothy Hoagland
//    NSCL / WIENER
//    s04.thoagland@wittenberg.edu
//
//  Adapted from the CTrueGate class written by
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

#include <config.h>
#include "MaskGates.h"                               
#include "SingleItemIterator.h"
#include <stdio.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Functions for class CMaskGate

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t operator() ( CEvent& rEvent )
//  Operation Type:
//     Evaluator.
//
Bool_t 
CMaskGate::operator()(CEvent& rEvent) 
{
  if(!wasChecked()) {
    Set(inGate(rEvent));
  }

  return getCachedValue();
}


//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator Begin (  )
//  Operation Type:
//     Selector
//
CConstituentIterator 
CMaskGate::Begin() 
{
// Returns an iterator which points to the
// beginning of our constituent list.  Since
// we have no constituent list, this function
// returns End()
// Exceptions:  
  CSingleItemIterator It;
  return CConstituentIterator(It);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator End (  )
//  Operation Type:
//     Selector
//
CConstituentIterator 
CMaskGate::End() 
{
  CConstituentIterator p = Begin();
  p++;
  return p;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t Size (  )
//  Operation Type:
//     Selector.
//
UInt_t 
CMaskGate::Size() 
{
// Returns the number of entries in the constituent
// list.  In this case, 0 is returned.
// Exceptions:  

  return 1;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string GetConstituent ( CConstituentIterator& rIterator )
//  Operation Type:
//     Selector
//
std::string 
CMaskGate::GetConstituent(CConstituentIterator& rIterator) 
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

  CConstituentIterator e = End();
  if((rIterator != e)) {
    char Text[100];
    sprintf(Text, "%d", m_nId);
    return std::string(Text);
  }
  else {
    return std::string("");
  }
}
/**
 *  getParameters
 *   @return std::vector<UInt_t> - single element vector with the parameter id.
 */
std::vector<UInt_t>
CMaskGate::getParameters() {
  std::vector<UInt_t> result;

  result.push_back(getId());
  return result;
}
/**
 *  getMask
 *    @return UInt_t - the mask.
 */
UInt_t
CMaskGate::getMask() {
  return getCompare();
}


