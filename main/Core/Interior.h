/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/
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
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __CPP__IOSTREAM_H
#include <iostream>
#define __CPP__IOSTREAM_H
#endif


                               //Required for 1:1 associated classes
#ifndef __INTERIORROW_H
#include "InteriorRow.h"
#endif                                                               
                                                               
class CInterior      
{
  
  std::vector<CInteriorRow> m_aRows;
  
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

  std::vector<CInteriorRow> getRows() const
  {
    return m_aRows;
  }
    
  // Mutators:

protected:
  void setRows (std::vector<CInteriorRow> am_aRows)
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
    std::vector<CInteriorRow>::iterator RowBegin ()  ;
    std::vector<CInteriorRow>::iterator RowEnd ()  ;

};

std::ostream& operator<<(std::ostream& of, CInterior& rInt);
#endif
