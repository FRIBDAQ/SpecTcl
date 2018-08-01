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

//  CButtonEvent.h:
//
//    This file defines the CButtonEvent class.
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

#ifndef BUTTONEVENT_H  //Required for current class
#define BUTTONEVENT_H
                               
#include "Point.h"
#include <histotypes.h>
#include <string>
#include <clientops.h>



//  Typedefs used by the class:


class CButtonEvent      
{
private:
  Int_t       m_nId;		   // ID of the pressed button
  Int_t       m_nCurrentSpectrum;  // Xamine's selected spectrum.
  Bool_t      m_fToggleState;	   // State of toggle if button is toggle.
  Int_t       m_nPromptedSpectrum; // Spectrum ID chosen from prompter
  std::string m_sPromptedString;   // Text from filename or Text prompter.
  PointArray  m_vPoints;	   // Array of accepted points.
  
public:
  // Constructor:

  CButtonEvent(const msg_ButtonPress& rButtonInfo) :
    m_nId(rButtonInfo.buttonid),
    m_nCurrentSpectrum(rButtonInfo.selected),
    m_fToggleState(rButtonInfo.togglestate),
    m_nPromptedSpectrum(rButtonInfo.spectrum),
    m_sPromptedString(rButtonInfo.text)
  {
    m_vPoints.clear();
    for(int i = 0; i < rButtonInfo.npts; i++) {
      m_vPoints.push_back(CPoint(rButtonInfo.points[i].x,
				 rButtonInfo.points[i].y));
    }
  }
  virtual  ~CButtonEvent ( ) { }       //Destructor

	
			//Copy constructor

  CButtonEvent (const CButtonEvent& aCButtonEvent ) 
  {   
    m_nId               = aCButtonEvent.m_nId;
    m_nCurrentSpectrum  = aCButtonEvent.m_nCurrentSpectrum;
    m_fToggleState      = aCButtonEvent.m_fToggleState;
    m_nPromptedSpectrum = aCButtonEvent.m_nPromptedSpectrum;
    m_sPromptedString   = aCButtonEvent.m_sPromptedString;
    m_vPoints           = aCButtonEvent.m_vPoints;
                
  }                                     

			//Operator= Assignment Operator

  CButtonEvent& operator= (const CButtonEvent& aCButtonEvent)
  { 
    if (this == &aCButtonEvent) return *this;          
    m_nId = aCButtonEvent.m_nId;
    m_nCurrentSpectrum = aCButtonEvent.m_nCurrentSpectrum;
    m_fToggleState = aCButtonEvent.m_fToggleState;
    m_nPromptedSpectrum = aCButtonEvent.m_nPromptedSpectrum;
    m_sPromptedString = aCButtonEvent.m_sPromptedString;
    m_vPoints           = aCButtonEvent.m_vPoints;
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CButtonEvent& aCButtonEvent)
  { 
    return (
	    
	    (m_nId == aCButtonEvent.m_nId) &&
	    (m_nCurrentSpectrum  == aCButtonEvent.m_nCurrentSpectrum) &&
	    (m_fToggleState      == aCButtonEvent.m_fToggleState) &&
	    (m_nPromptedSpectrum == aCButtonEvent.m_nPromptedSpectrum) &&
	    (m_sPromptedString   == aCButtonEvent.m_sPromptedString)  &&
	    (m_vPoints           == aCButtonEvent.m_vPoints)
	    );
  }                             
  // Selectors:

public:
  Int_t getId() const
  {
    return m_nId;
  }
  Int_t getCurrentSpectrum() const
  {
    return m_nCurrentSpectrum;
  }
  Bool_t getToggleState() const
  {
    return m_fToggleState;
  }
  Int_t getPromptedSpectrum() const
  {
    return m_nPromptedSpectrum;
  }
  std::string getm_sPromptedString() const
  {
    return m_sPromptedString;
  }
  const PointArray& getPoints() const
  {
    return m_vPoints;
  }
  // Mutators (available to derived classes):

protected:  

  void setId (Int_t am_nId)
  { 
    m_nId = am_nId;
  }
  void setCurrentSpectrum (Int_t am_nCurrentSpectrum)
  { 
    m_nCurrentSpectrum = am_nCurrentSpectrum;
  }
  void setToggleState (Bool_t am_fToggleState)
  { 
    m_fToggleState = am_fToggleState;
  }
  void setPromptedSpectrum (Int_t am_nPromptedSpectrum)
  { 
    m_nPromptedSpectrum = am_nPromptedSpectrum;
  }
  void setPromptedString (std::string am_sPromptedString)
  { 
    m_sPromptedString = am_sPromptedString;
  }
  void setPoints(const PointArray& rPoints)
  {
    m_vPoints = rPoints;
  }
  //  Operations:

public:
  PointIterator begin () {
    return m_vPoints.begin();
  }

  PointIterator end () {
    return m_vPoints.end();
  }

  UInt_t size () const {
    return m_vPoints.size();
  }
  const CPoint& operator[] (UInt_t nI)  const {
    return m_vPoints[nI];
  }
 
};

#endif
