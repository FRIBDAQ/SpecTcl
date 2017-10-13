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
//  CXamine2D.h:
//
//    This file defines the CXamine2D class.
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

/*
   Change log:
   $Log$
   Revision 5.3  2005/09/22 12:41:47  ron-fox
   2dl spectra in Xamine and other misc stuff.. including making
   void functions return values in all paths, including exception
   exits since g++3.x and higher likes that.

   Revision 5.2  2005/06/03 15:19:35  ron-fox
   Part of breaking off /merging branch to start 3.1 development

   Revision 5.1.2.1  2004/12/21 17:51:29  ron-fox
   Port to gcc 3.x compilers.

   Revision 5.1  2004/11/29 16:56:19  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 4.3  2003/04/03 00:04:19  ron-fox
   These files are produced during Make and therefore are not needed.

   Revision 4.2  2003/04/01 19:48:22  ron-fox
   Support for Real valued parameters and spectra with arbitrary binnings.

*/

#ifndef __XAMINE2D_H  //Required for current class
#define __XAMINE2D_H
                               //Required for base classes
#ifndef __XAMINESPECTRUM_H
#include "XamineSpectrum.h"
#endif

#ifndef __XAMINEMAP2D_H
#include "XamineMap2D.h"
#endif

#ifndef __XAMINE_XAMINE_H
#include <Xamine.h>
#define __XAMINE_XAMINE_H
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef  __STL_STRING
#define  __STL_STRING
#endif
#endif


class CXamine1D;		// Forward definition.

/*!
    Encapsulates a 2d spectrum that's held in Xamine's shared memory
    region.  Construction can be done either one or two stage... either
    feed all the info right away or get a object and attach it to an
    existing spectrum in Xamine.
*/

class CXamine2D  : public CXamineSpectrum        
{
  Int_t m_nType;		//!< 0 - word 1 byte 2 longs.
  UInt_t m_nXchannels;		//!< no. of X channels.
  UInt_t m_nYchannels;		//!< No of y Channels.
  CXamineMap2D m_XamineMap;     //!< Transformation information, if applicable
  
public:
  // Constructor:

  CXamine2D(volatile Xamine_shared* pXamine, const std::string& rName,
	    UInt_t nXchans, UInt_t nYchans, Int_t nType=2) :
    CXamineSpectrum(pXamine, rName),
    m_nType(nType),
    m_nXchannels(nXchans),
    m_nYchannels(nYchans),
    m_XamineMap(0.0,0.0,0.0,0.0,std::string(""),std::string(""))
    {}
  CXamine2D(volatile Xamine_shared* pXamine, 
	    const std::string& rName,
	    UInt_t nXChans, UInt_t nYChans, 
	    Float_t nXLow, Float_t nYLow,
	    Float_t nXHigh, Float_t nYHigh,
	    const std::string& sXUnits, const std::string& sYUnits,
	    Int_t nType = 2) : 
    CXamineSpectrum(pXamine, rName),
    m_nType(nType),
    m_nXchannels(nXChans),
    m_nYchannels(nYChans),
    m_XamineMap(nXLow, nYLow, nXHigh, nYHigh, sXUnits, sYUnits)
    {}

  CXamine2D(volatile Xamine_shared* pXamine, UInt_t  nSlot);
  virtual ~ CXamine2D ( ) { }       //Destructor
	
  //Copy constructor
  CXamine2D (const CXamine2D& aCXamine2D )   : 
    CXamineSpectrum (aCXamine2D) 
  {   
    m_nType      = aCXamine2D.m_nType;
    m_nXchannels = aCXamine2D.m_nXchannels;
    m_nYchannels = aCXamine2D.m_nYchannels;
    m_XamineMap  = aCXamine2D.m_XamineMap;
  }                                     
  
  //Operator= Assignment Operator
  
  CXamine2D& operator= (const CXamine2D& aCXamine2D)
  { 
    if (this == &aCXamine2D) return *this;          
    CXamineSpectrum::operator= (aCXamine2D);
    m_nType      = aCXamine2D.m_nType;
    m_nXchannels = aCXamine2D.m_nXchannels;
    m_nYchannels = aCXamine2D.m_nYchannels;
    m_XamineMap  = aCXamine2D.m_XamineMap;
    
    return *this;
  }                                     
  
  //Operator== Equality Operator
  
  int operator== (const CXamine2D& aCXamine2D)
  { 
    return (
	    (CXamineSpectrum::operator== (aCXamine2D)) &&
	    
	    (m_nType      == aCXamine2D.m_nType) &&
	    (m_nXchannels == aCXamine2D.m_nXchannels) &&
	    (m_nYchannels == aCXamine2D.m_nYchannels) &&
	    (m_XamineMap  == aCXamine2D.m_XamineMap)
	    );
  }           
  // Selectors:                  
  
public:
  Int_t getType() const
  {
    return m_nType;
  }
  UInt_t getXchannels() const
  {
    return m_nXchannels;
  }
  UInt_t getYchannels() const
  {
    return m_nYchannels;
  }
  CXamineMap2D getXamineMap() const
    {
      return m_XamineMap;
    }
  // Mutators are available only to derived classes:

protected:                       

  void setType (Int_t am_nType)
  { 
    m_nType = am_nType;
  }
  void setXchannels (UInt_t am_nXchannels)
  { 
    m_nXchannels = am_nXchannels;
  }
  void setYchannels (UInt_t am_nYchannels)
  { 
    m_nYchannels = am_nYchannels;
  }
  void setXamineMap (CXamineMap2D& am_XamineMap) 
    {
      m_XamineMap = am_XamineMap;
    }
  // operations on the class:

public:
  virtual   CXamine1D* Oned ()  ;
  virtual   CXamine2D* Twod ()  ;
};

#endif
