//  CCut.h:
//
//    This file defines the CCut class.
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

#ifndef __CUT_H  //Required for current class
#define __CUT_H
                               //Required for base classes
#ifndef __GATE_H
#include "Gate.h"
#endif

#ifndef __STRING
#include <string>
#define __STRING
#endif
                               
class CCut  : public CGate        
{
  UInt_t m_nLow;  // low level of cut (inclusive)
  UInt_t m_nHigh;  // High level of cut (inclusive)
  UInt_t m_nId;  // Id of parameter to cut.
  
public:
			//Default constructor

  CCut (  UInt_t am_nLow,  UInt_t am_nHigh,  UInt_t am_nId  )  :   
    m_nLow (am_nLow),  
    m_nHigh (am_nHigh),  
    m_nId (am_nId)  
    { }        
  ~ CCut ( ) { }       //Destructor

	
			//Copy constructor

  CCut (const CCut& aCCut )   : CGate (aCCut) 
  {   
    m_nLow = aCCut.m_nLow;
    m_nHigh = aCCut.m_nHigh;
    m_nId = aCCut.m_nId;               
  }                                     

			//Operator= Assignment Operator

  CCut& operator= (const CCut& aCCut)
  { 
    if (this == &aCCut) return *this;          
    CGate::operator= (aCCut);
    m_nLow = aCCut.m_nLow;
    m_nHigh = aCCut.m_nHigh;
    m_nId = aCCut.m_nId;
    
    return *this;                                                                                                 
  }                                     

			//Operator== Equality Operator
private:
  int operator== (const CCut& aCCut);
public:
  // Selectors

public:				//Get accessor function for attribute
  UInt_t getLow() const
  {
    return m_nLow;
  }

                       //Get accessor function for attribute
  UInt_t getHigh() const
  {
    return m_nHigh;
  }

                       //Get accessor function for attribute
  UInt_t getId() const
  {
    return m_nId;
  }

  virtual vector<string> getSpecs() const { }

  // Mutators.
                       //Set accessor function for attribute
protected:
  void setm_nLow (UInt_t am_nLow)
  { 
    m_nLow = am_nLow;
  }

                       //Set accessor function for attribute
  void setm_nHigh (UInt_t am_nHigh)
  { 
    m_nHigh = am_nHigh;
  }

                       //Set accessor function for attribute
  void setm_nId (UInt_t am_nId)
  { 
    m_nId = am_nId;
  }

  // Member functions of the class.

public:                   
  virtual   Bool_t operator()(CEvent& rEvent);
  virtual   CGate* clone ()  ;
  virtual   CConstituentIterator Begin ()  ;
  virtual   CConstituentIterator End ()  ;
  virtual   UInt_t Size ()  ;
  virtual   std::string GetConstituent (CConstituentIterator& rIterator)  ;
  virtual   std::string Type ()  const;
  virtual   Bool_t inGate(CEvent& rEvent, const vector<UInt_t>& Params);
  virtual   Bool_t inGate (CEvent& rEvent);
};

#endif
