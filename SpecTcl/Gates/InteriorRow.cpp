//  CInteriorRow.cpp
// Encapsulates a scanline of an interior.
// The scan line contains words of at least 32 bits
// with each bit corresponding to a flag indicating if a
// position is inside or outside the figure described by the
// containing interior.

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


#include "InteriorRow.h"                               

static const char* Copyright = 
"CInteriorRow.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CInteriorRow

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t Size (  )
//  Operation Type:
//     Selector
//
UInt_t 
CInteriorRow::Size() 
{
// Returns the number of elements in a row (not # of pixels).
// Exceptions:  

  return m_vRow.size();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    vector<UInt_t>::iterator Begin (  )
//  Operation Type:
//     Selector.
//
vector<UInt_t>::iterator 
CInteriorRow::Begin() 
{
// Returns an iterator to the row.
// The iterator iterates over the integers, not the pixels.
// Exceptions:  

  return m_vRow.begin();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    vector<UInt_t>::iterator End (  )
//  Operation Type:
//     Selector
//
vector<UInt_t>::iterator 
CInteriorRow::End() 
{
// Returns an end matching iterator for the
// row.
// Exceptions:  

  return m_vRow.end();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t& operator[] ( UInt_t i )
//  Operation Type:
//     Selector/Mutator
//
UInt_t& 
CInteriorRow::operator[](UInt_t i) 
{
// Returns a reference to the I'th element
// of the row.
//
// Formal Parameters:
//   UInt_t i:
//     Index.

// Exceptions:  

  return m_vRow[i];
}
