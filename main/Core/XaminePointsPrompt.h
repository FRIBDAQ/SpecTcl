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
//  CXaminePointsPrompt.h:
//
//    This file defines the CXaminePointsPrompt class.
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

#ifndef XAMINEPOINTSPROMPT_H  //Required for current class
#define XAMINEPOINTSPROMPT_H
                               //Required for base classes
#include "XamineButtonPrompt.h"
#include <string>
#include <histotypes.h>
                               
class CXaminePointsPrompt  : public CXamineButtonPrompt        
{
  UInt_t m_nMinPoints;		// Minimum number of acceptable points.
  UInt_t m_nMaxPoints;		// Maximum number of acceptable points.
  
public:
			//Default constructor

  CXaminePointsPrompt (const std::string& rPrompt, 
		       UInt_t nMinPoints = 1,
		       UInt_t nMaxPoints = GROBJ_MAXPTS) :
    CXamineButtonPrompt(rPrompt),
    m_nMinPoints(nMinPoints),
    m_nMaxPoints(nMaxPoints)
  { }
  CXaminePointsPrompt(const char* pPrompt,
		      UInt_t nMinPoints = 1,
		      UInt_t nMaxPoints = GROBJ_MAXPTS) :
    CXamineButtonPrompt(pPrompt),
    m_nMinPoints(nMinPoints),
    m_nMaxPoints(nMaxPoints)
  { }
  virtual  ~ CXaminePointsPrompt ( ) { }       //Destructor

			//Copy constructor

  CXaminePointsPrompt (const CXaminePointsPrompt& aCXaminePointsPrompt )   : 
    CXamineButtonPrompt (aCXaminePointsPrompt) 
  {   
    m_nMinPoints = aCXaminePointsPrompt.m_nMinPoints;
    m_nMaxPoints = aCXaminePointsPrompt.m_nMaxPoints;           
  }                                     

			//Operator= Assignment Operator

  CXaminePointsPrompt& operator= 
                            (const CXaminePointsPrompt& aCXaminePointsPrompt)
  { 
    if (this == &aCXaminePointsPrompt) return *this;          
    CXamineButtonPrompt::operator= (aCXaminePointsPrompt);
    m_nMinPoints = aCXaminePointsPrompt.m_nMinPoints;
    m_nMaxPoints = aCXaminePointsPrompt.m_nMaxPoints;
    
    return *this;
  }                                     

			//Operator== Equality Operator
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  int operator== (const CXaminePointsPrompt& aCXaminePointsPrompt)
  { 
    return (
	    (CXamineButtonPrompt::operator== (aCXaminePointsPrompt)) &&
	    (m_nMinPoints == aCXaminePointsPrompt.m_nMinPoints) &&
	    (m_nMaxPoints == aCXaminePointsPrompt.m_nMaxPoints) 
	    );
  }      
  // Selectors:

public:

  int getMinPoints() const
  {
    return m_nMinPoints;
  }
  int getMaxPoints() const
  {
    return m_nMaxPoints;
  }
  // Mutators:

protected:
  void setMinPoints (int am_nMinPoints)
  { 
    m_nMinPoints = am_nMinPoints;
  }

                       //Set accessor function for attribute
  void setMaxPoints (int am_nMaxPoints)
  { 
    m_nMaxPoints = am_nMaxPoints;
  }
                       
  virtual   void FormatPrompterBlock (ButtonDescription& rButton) const  ;
 
};

#endif
