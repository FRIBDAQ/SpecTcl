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

//  CXamine.h:
//
//    This file defines the CXamine class.
//    Note this file does not adheare to file nameing conventions to prevent
//    clashes with <Xamine.h>  Xamineplus is short for the C++ bindings
//    to the Xamine API.
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

#ifndef __XAMINEPLUS_H  //Required for current class
#define __XAMINEPLUS_H

#ifndef __XAMINE_XAMINE_H
#include <Xamine.h>
#define __XAMINE_XAMINE_H
#endif


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __XAMINESPECTRUMITERATOR_H
#include "XamineSpectrumIterator.h"
#endif


//
//  forward references to classes:
//
class CXamineGates;		// Forward reference.
class CDisplayGate;
class CXamineEvent;
class CXamineSpectrum;
class CXamineButton;
                               
class CXamine      
{
   volatile Xamine_shared* m_pDisplay;  //  Pointer to Xamine shared memory region.
   Bool_t         m_fManaged;  //  Set TRUE if memory management started. 
   UInt_t         m_nBytes;    //  Size of shared memory region.
  
public:
  // Constructors:

  CXamine();			// Bind to existing Xamine
  CXamine (UInt_t nBytes);	
				// New Xamine region nbytes of spectrum space
  ~CXamine ( ) { }       //Destructor

	
				// Copy Constructor.

  CXamine (const CXamine& aCXamine ) :
    m_pDisplay(aCXamine.m_pDisplay),
    m_fManaged(aCXamine.m_fManaged),
    m_nBytes(aCXamine.m_nBytes)
  {   
                
  }                                     

			//Operator= Assignment Operator

  CXamine& operator= (const CXamine& aCXamine)
  { 
    if (this == &aCXamine) return *this;          
    m_pDisplay = aCXamine.m_pDisplay;
    m_fManaged = aCXamine.m_fManaged;
    m_nBytes   = aCXamine.m_nBytes;
        
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamine& aCXamine)
  { 
    return (
	    (m_pDisplay == aCXamine.m_pDisplay) 
	    );
  }                             
  // Selectors:

public:
  volatile Xamine_shared* getXamineMemory() const
  {
    return m_pDisplay;
  }
                       
  // Mutator (for derivec classes):

protected:
  void setXamineMemory (Xamine_shared* am_pDisplay)
  { 
    m_pDisplay = am_pDisplay;
  }
  //
  //  Bindings to the Xamine API
  //
public:                       
  STD(string) GetMemoryName ()  ;  
  void MapMemory (const STD(string)& rsName, 
		  UInt_t nBytes=knDefaultSpectrumSize)  ;
  Bool_t isAlive ()  ;  
  void Start ()  ;
  void Stop ()  ;
  void Restart();
  void EnterGate (CDisplayGate& rGate)  ;
  void RemoveGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType)  ;
  void EnterPeakMarker (UInt_t nSpectrum, 
			UInt_t nId, 
			const STD(string)& rsName, 
			Float_t fCentroid, Float_t fWidth)  ;
  CXamineGates* GetGates (UInt_t nSpectrum)  ;
  UInt_t GetEventFd ()  ;
  Bool_t PollEvent (Int_t nTimeout, CXamineEvent& rEvent)  ;
  Address_t DefineSpectrum (CXamineSpectrum& rSpectrum)  ;
  void FreeSpectrum (UInt_t nSpectrum)  ;
  void DefineButtonBox (UInt_t nColumns=8, UInt_t NRows=3)  ;
  void DefineButton (UInt_t nColumn, UInt_t nRow, 
			    const CXamineButton& rButton)  ;
  void ModifyButton (UInt_t nColumn, UInt_t nRow, 
			    const CXamineButton& rButton)  ;
  void EnableButton (UInt_t nColumn, UInt_t nRow)  ;
  void DisableButton (UInt_t  nColumn, UInt_t nRow)  ;
  void DeleteButton (UInt_t nColumn, UInt_t nRow)  ;
  void DeleteButtonBox ()  ;
  CXamineButton* InquireButton (UInt_t nColumn, UInt_t nRow)  ;

  CXamineSpectrum& operator[] (UInt_t n)  ;
  CXamineSpectrumIterator begin ()  ;
  CXamineSpectrumIterator end ()  ;

  UInt_t getTitleSize()  const;
  void setTitle(STD(string) name, UInt_t slot);
  void setInfo(STD(string)  name, UInt_t slot);

  // Utility Functions:

protected:
  void ThrowGateStatus(Int_t nStatus, const CDisplayGate& rGate, 
			      const STD(string)& doing);
  void ThrowButtonStatus(Int_t nStatus, const CXamineButton& rButton,
				const STD(string)& doing);
  DialogSpectrumType_t MaptoSpec_t(ButtonDialogSpectrumType t);
  Xamine_gatetype      MapFromGate_t(GateType_t type);
};

#endif
