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

//
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved TCLHistogrammer.h
//

#ifndef TCLHISTOGRAMMER_H  //Required for current class
#define TCLHISTOGRAMMER_H

#include <PseudoScript.h>
#include "Histogrammer.h"
#include "TCLInterpreterObject.h"
#include "PseudoScript.h"
#include <list>
  
typedef std::list<CPseudoScript>                   PseudoParameters;
typedef std::list<CPseudoScript>::iterator         PseudoParameterIterator;
                                                           
class CTCLHistogrammer  : public CHistogrammer        
{                       
	
private:		
  CTCLInterpreter*       m_pIntObject; //1:1 association object data member 
  PseudoParameters       m_Pseudos;


public:

   // Constructors and other cannonical operations:

  CTCLHistogrammer (CTCLInterpreter* m_pInterpreter);
  virtual ~ CTCLHistogrammer ( );  
  
   //Copy constructor 
private:
  CTCLHistogrammer (const CTCLHistogrammer& aCTCLHistogrammer ) ;

   // Operator= Assignment Operator 

  CTCLHistogrammer& operator= (const CTCLHistogrammer& aCTCLHistogrammer);
 
   //Operator== Equality Operator 

  int operator== (const CTCLHistogrammer& aCTCLHistogrammer);
	
// Selectors:

public:
                       
  CTCLInterpreter* getInterpreter() const
  { 
    return m_pIntObject;
  }
  PseudoParameters getPseudoParameters() const
  {
    return m_Pseudos;
  }
                       
// Mutators:

protected:
       
  void setInterpreter(CTCLInterpreter* am_pIntObject)
  { 
    m_pIntObject = am_pIntObject;
  }
  void setPseudoParameters(const PseudoParameters& rPseudos) {
    m_Pseudos = rPseudos;
  }

public:
  virtual   void operator() (const  CEvent& rEvent);

  void AddPseudo (CPseudoScript& rScript)    ;
  void RemovePseudo (const std::string& rPseudoName)    ;
  CPseudoScript& FindPseudo (const std::string& rPsuedoName)    ;
  PseudoParameterIterator PseudoBegin ()    ;
  PseudoParameterIterator PseudoEnd ()    ;
  UInt_t PseudoSize ()    ;
};

#endif
