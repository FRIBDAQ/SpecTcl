//  CInterior.h:
//
//    This file defines the CInterior class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __INTERIOR_H  //Required for current class
#define __INTERIOR_H
          
#ifndef __STL_VECTOR                     
#include <vector>   //Required for include files, eg <CList.h>
#define __STL_VECTOR
#endif

#ifndef __CPP__IOSTREAM_H
#include <iostream.h>
#define __CPP__IOSTREAM_H
#endif


                               //Required for 1:1 associated classes
#ifndef __INTERIORROW_H
#include "InteriorRow.h"
#endif                                                               
                                                               
class CInterior      
{
  
  vector<CInteriorRow> m_aRows;
  
public:
			// Constructors

  CInterior () {}
  CInterior (UInt_t nr,UInt_t nc);
  virtual ~ CInterior ( ) { }       //Destructor

                       // Copy Constructor.
  CInterior (const CInterior& aCInterior )
  { 
    m_aRows = aCInterior.m_aRows;
  }                                     

			//Operator= Assignment Operator

  CInterior& operator= (const CInterior& aCInterior);


			//Operator== Equality Operator

  int operator== (const CInterior& aCInterior);
                       
  // Selectors.

  vector<CInteriorRow> getRows() const
  {
    return m_aRows;
  }
    
  // Mutators:

protected:
  void setRows (vector<CInteriorRow> am_aRows)
  { 
    m_aRows = am_aRows;
  }
public:
  void setShape(UInt_t nr, UInt_t nc);

  //
  // Operations on the class.
  //
public:
    void Clear ()  ;
    void Set (UInt_t  nX, UInt_t nY, Bool_t fValue)  ;
    Bool_t Inside (UInt_t  nX, UInt_t nY)  ;
    UInt_t getRowSize (UInt_t  nRow)  ;
    UInt_t getRowCount ()  ;
    vector<CInteriorRow>::iterator RowBegin ()  ;
    vector<CInteriorRow>::iterator RowEnd ()  ;

};

ostream& operator<<(ostream& of, CInterior& rInt);
#endif
