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

//  CRunControl.h:
//
//    This file defines the CRunControl class.
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

#ifndef __RUNCONTROL_H  //Required for current class
#define __RUNCONTROL_H
                               
//Required for 1:1 associated classes
#ifndef __ANALYZER_H
#include "Analyzer.h"
#endif
//Required for 1:1 associated classes
#ifndef __FILE_H
#include "File.h"
#endif                                                               

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif          
                                                     
class CRunControl {
  // Attributes:
 private:
  Bool_t     m_fRunning;	// kfTRUE if playback is in progress.
  CAnalyzer* m_pAnalyzer;	// Pointer to the run's event analyzer.
  CFile*     m_pEventSource;	// Pointer to the run's event source.

 public:
  // Constructors and destructor.
  CRunControl (CAnalyzer& rAnalyzer, CFile& rEventSource) :
  m_fRunning(kfFALSE),
    m_pAnalyzer(&rAnalyzer),
    m_pEventSource(&rEventSource)
    {}


  CRunControl (const CRunControl& aCRunControl) { // Copy constructor.
    m_fRunning    = aCRunControl.m_fRunning;
    m_pAnalyzer   = aCRunControl.m_pAnalyzer;
    m_pEventSource= aCRunControl.m_pEventSource;
  }

  virtual ~CRunControl() {} // Destructor.

  // Operators.
  CRunControl& operator=(const CRunControl& aCRunControl) {
    if (this == &aCRunControl) return *this;

    m_fRunning    = aCRunControl.m_fRunning;
    m_pAnalyzer   = aCRunControl.m_pAnalyzer;
    m_pEventSource= aCRunControl.m_pEventSource;

    return *this;
  }

  int operator==(const CRunControl& aCRunControl) {
    return (
	    (m_fRunning    == aCRunControl.m_fRunning)         &&
	    (m_pAnalyzer   == aCRunControl.m_pAnalyzer)        &&
	    (m_pEventSource== aCRunControl.m_pEventSource)
	    );
  }

  // Selectors:
 public:
  Bool_t getRunning() const {
    return m_fRunning;
  }

  const CAnalyzer* getAnalyzer() const {
    return m_pAnalyzer;
  }

  const CFile* getEventSource() const {
    return m_pEventSource;
  }

  // Mutators:
 protected:
  void setRunning(Bool_t am_fRunning) {
    m_fRunning = am_fRunning;
  }

 public:
  void setAnalyzer(CAnalyzer* pAnalyzer) {
    m_pAnalyzer = pAnalyzer;
  }

  void setEventSource(CFile* pEventSource) {
    m_pEventSource = pEventSource;
  }

  // Public interface:
 public:
  virtual void Start() {
    m_fRunning = kfTRUE;
  }

  virtual void Stop() {
    m_fRunning = kfFALSE;
  }

  CFile* Attach(CFile* pNewfile);
  virtual void OnBuffer(UInt_t nBytes);
  virtual void OnEnd();
};

#endif
