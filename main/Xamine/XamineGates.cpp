/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/

static const char* Copyright = "(C) Copyright Michigan State University 2011, All rights reserved";
//  CXamineGates.cpp
// Encapsulates the results of the
// Xamine_StartSearch
// Xamine_NextGate
// Xamine_EndSarch
//
// Functions in a convenient  iterator
// modelled object oriented way. 
//
// Strategy:
//    On construction or temporally close to there, the
//    gates are gathered into m_vGates.  
//    Using Xamine_StartSearch and Xamine_Nextgate
//    and finishing with Xamine_EndSearch.
//    m_vGates is then an stl vector of CDisplayGate objects.

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
#include <config.h>

#include "XamineGates.h"                               
#include "RangeError.h"
#include <Xamine.h>		// Xamine C api.
#include <assert.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Functions for class CXamineGates

//////////////////////////////////////////////////////////////////////////
//
// Function:
//     CXamineGates()
// Operation Type:
//     Constructor
//
CXamineGates::CXamineGates(UInt_t spec)
{
  int status;

  void* context = Xamine_StartSearch(spec, &status);
  if(status != Success) return;	// All errors lead to an empty gate list.


  msg_object gate;
  gate.spectrum = spec;		// All gates from same spectrum.
  gate.hasname = 0;		// No names passed through this interface.
  while(Xamine_NextGate(context, (Int_t*)&(gate.id), &(gate.type),
			&(gate.npts), (Xamine_point*)gate.points)) {
    m_vGates.push_back(CDisplayGate(gate));
  }
  Xamine_EndSearch(context); 
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CDisplayGateVectorIterator begin (  )
//  Operation Type:
//     selector.
//
CDisplayGateVectorIterator 
CXamineGates::begin() 
{
// Returns an iterator which 'points' to the start of
// the m_vGates vector.  Tyipcally this is used
// to sequentially index through the gates
// as follows:
//     for(CDisplayGateVectorIterator i = Gates.begin();
//             i != Gates.end(); i++) {
//                // *i gets references to one of the gates.
//      }

  return m_vGates.begin();

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CDisplayGateVectorIterator end (  )
//  Operation Type:
//     selector.
//
CDisplayGateVectorIterator 
CXamineGates::end() 
{
// Returns an iterator to 'just past the end'
// of the m_vGates vector.
// See comments in begin() for how these iterators
// are typically used.

  return m_vGates.end();

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CDisplayGate& operator[] ( UInt_t n )
//  Operation Type:
//     selector.
//
CDisplayGate& 
CXamineGates::operator[](UInt_t n) 
{
// Returns the n'th element of the gate array or throws a 
//  CRangeError exception if i is too big.
//
// Formal Parameters:
//    UInt_t n:
//       Element of the gate array requested.
// Returns:
//    CDisplayGate& reference to the n'th gate.
//    note that this reference is not const so you can
//    modify the gate, and e.g. feed it back in to Xamine.
//
  if(n > m_vGates.size()) {
    throw CRangeError(0, m_vGates.size(), n,
		      "CXamineGates::operator[] - indexing gate array");
  }
  return m_vGates[n];

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t size (  )
//  Operation Type:
//     Selector
//
UInt_t 
CXamineGates::size() 
{
// Returns the number of elements in the 
// m_vGates vector.  So another way to 
// sequentially reference elements of the
// CXamineGates.m_vGates array is:
//
//   for(UInt i = 0; i < Gates.size(); i++) {
//       // do things with Gates[i]
//   }

  return m_vGates.size();

}
