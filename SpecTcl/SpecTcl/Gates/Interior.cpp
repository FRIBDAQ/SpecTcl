//  CInterior.cpp
// Encapsulates a 'normalized' 2-d interior region.
// An interior consists of a rectangular array 'flags'
// each entry in the flag arra indicates if the corresponding
// point is inside the interior.
//   Interiors are intended to be used as rapid lookups
// for 2-d contour regions (see CContour).
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


#include "Interior.h"                               
#include <algorithm>   //Required for include files, eg <CList.h>             
#include <assert.h>
#include <stdio.h>


static const UInt_t BITSPERWORD = sizeof(UInt_t)*8;

static const char* Copyright = 
"CInterior.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CInterior
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   CInterior(UInt_t nr, UInt_t nc)
// Operation type:
//    Construction
//
CInterior::CInterior(UInt_t nr, UInt_t nc)
{
  setShape(nr,nc);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//     void setShape(UInt_t nr, UInt_t nc)
//  Operation type:
//     Mutator.
//
void
CInterior::setShape(UInt_t nr, UInt_t nc)
{
  CInteriorRow empty;
  m_aRows.reserve(nr);
  m_aRows.insert(m_aRows.begin(), (int)nr, empty);
  UInt_t rs = (nc + BITSPERWORD - 1) / BITSPERWORD;
  for(UInt_t i = 0; i < nr; i++) {
    m_aRows[i].SetRowSize(rs);
    fill(m_aRows[i].Begin(), m_aRows[i].End(), 0);
  }
#ifdef __DEBUG__
  cout << "Interior rows = " << nr;
  cout << "         cols = " << rs << endl;
#endif
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Clear (  )
//  Operation Type:
//     Initialization
//
void 
CInterior::Clear() 
{
  //Initializes the interior to exclude all points.
  // Exceptions:  

  for(UInt_t row = 0; row < m_aRows.size(); row++) {
     fill(m_aRows[row].Begin(), m_aRows[row].End(), 0);
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Set ( UInt_t  nX, UInt_t nY, Bool_t fValue )
//  Operation Type:
//     Mutator
//
void 
CInterior::Set(UInt_t  nX, UInt_t nY, Bool_t fValue) 
{
// Sets a specific coordinate to be either inside or outside the 
// region.  
//    Implementation note:  Each coordinate flag is a single bit of
// a word which is at least 32 bits long.
//
// Formal parameters:
//    UInt_t nX, nY:
//       Coordinates of the point.
//   Bool_t kfValue:
//       kfTRUE for interior, kfFALSE if exterior point.
//
// Exceptions:  


  UInt_t Row    = nY;
  UInt_t CCell  = nX / BITSPERWORD;
  UInt_t BitNo  = nX % BITSPERWORD;
  UInt_t BitMask= 1 << BitNo;

#ifdef __DEBUG__
  cout << "Set (" << nX << "," << nY << ") Rowsize= " << 
    m_aRows[Row].Size() << endl;
  cout.flush();
#endif
  
  assert(Row   < getRowCount());              // Be sure we're inside the
  assert(m_aRows[Row].Size() > 0);
  assert(CCell < m_aRows[Row].Size());       // array.
  
  m_aRows[Row][CCell] &= ~BitMask;           // Clear prior bit.
  if(fValue) 
    m_aRows[Row][CCell] |= BitMask;          // Set if necessary.
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t Inside ( UInt_t  nX, UInt_t nY )
//  Operation Type:
//     Selector.
//
Bool_t 
CInterior::Inside(UInt_t  nX, UInt_t nY) 
{
// Determines if the specified coordinate is 'inside' the interior.
//
// Formal Parameters:
//   UInt_t nX, nY:
//     Coordinates to check.
//
// Returns:
//   kfTRUE    - Inside.
//  kfFALSE    - Outside.
//
// Exceptions:  

  UInt_t Row   = nY;
  if(Row >= m_aRows.size()) 
     return kfFALSE;   // Too big is outside.
  
  UInt_t CCell = nX / BITSPERWORD;
  if(CCell >= m_aRows[Row].Size()) 
     return kfFALSE; // Too big is outside.
  
  UInt_t nBit = nX % BITSPERWORD;
  UInt_t nMask= 1 << nBit;
  UInt_t cell = m_aRows[Row][CCell];
  
  return ((cell & nMask) ? kfTRUE : kfFALSE);
  
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t getRowSize ( UInt_t  nRow )
//  Operation Type:
//     Selector.
//
UInt_t 
CInterior::getRowSize(UInt_t  nRow) 
{
// Returns the size of a given row.
// 0 is returned if the row does not exist.
//
// Formal Parameters:
//    UInt_t nRow
//
// Exceptions:  

  if(nRow >= m_aRows.size()) return 0;
  return m_aRows[nRow].Size();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t getRowCount (  )
//  Operation Type:
//     Selector.
//
UInt_t 
CInterior::getRowCount() 
{
// Returns the number of rows in the interior.
// Exceptions:  

  return m_aRows.size();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    vector<CInteriorRow>::iterator RowBegin (  )
//  Operation Type:
//     selector.
//
vector<CInteriorRow>::iterator 
CInterior::RowBegin() 
{
// Returns an iterator allowing individual rows to
// be fetched.
// Exceptions:  

  return m_aRows.begin();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    vector<CInteriorRow>::iterator RowEnd (  )
//  Operation Type:
//     Selector.
//
vector<CInteriorRow>::iterator 
CInterior::RowEnd() 
{
// Returns an end test iterator for the rows in the interior.
// Exceptions:  

  return m_aRows.end();
}
////////////////////////////////////////////////////////////////////////////
// 
// Function:
//    CInterior& operator= (const CInterior& aCInterior)
// Operation Type:
//    Assignment:
//
CInterior&
CInterior::operator= (const CInterior& aCInterior)
{
   if(this != &aCInterior) {            // Need to do the assign.
      m_aRows = aCInterior.m_aRows;
   }
   return *this;
}
////////////////////////////////////////////////////////////////////////
//
// Function:
//   ostream& operator<<(ostream& of, CInterior& rInt)
// Operation type:
//   Debugging I/O
//
ostream& 
operator<<(ostream& of, CInterior& rInt)
{
  of << "-------------- CInterior dump -----------" << endl;
  of << " Interior of " << rInt.getRowCount() << " rows" << endl;
  for(UInt_t i = 0; i < rInt.getRowCount(); i++) {
    of << "Row " << i << " Has " << rInt.getRowSize(i)*BITSPERWORD << endl;
    for(UInt_t j = 0; j < rInt.getRowSize(i)*BITSPERWORD; j++) {
      of  << (rInt.Inside(j,i) ? '1' : '0');
    }
    of  << endl;
  }
  return of;
}
