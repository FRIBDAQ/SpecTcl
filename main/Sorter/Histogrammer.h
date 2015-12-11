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

//  CHistogrammer.h:
//
//    This file defines the CHistogrammer class.
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

#ifndef __HISTOGRAMMER_H  //Required for current class
#define __HISTOGRAMMER_H

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif


#ifndef __STL_LIST
#include <list>
#ifndef __STL_LIST
#define __STL_LIST
#endif
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#define __HISTOTYPES_H
#endif

#ifndef __PARAMETER_H
#include "Parameter.h"
#define __PARAMETER_H
#endif

#ifndef __SPECTRUM_H
#include "Spectrum.h"
#define __SPECTRUM_H
#endif

#ifndef __DICTIONARY_H
#include "Dictionary.h"
#define __DICTIONARY_H
#endif

#ifndef __EVENTSINK_H
#include "EventSink.h"
#define __EVENTSINK_H
#endif

#ifndef __GATECONTAINER_H
#include <GateContainer.h>
#define __GATECONTAINER_H
#endif

// Forward class definitions (probably should be a lot more of these).

class CSpectrumFit;
class CHistogrammerFitObserver;
class CFlattenedGateList;
class CSpectrumByParameter;
class CDisplayInterface;
class CDisplayGate;

// Typedefs for some of instances of templated classes:
// Dictionary types:
typedef CDictionary<CParameter>                 ParameterDictionary;
typedef ParameterDictionary::DictionaryIterator ParameterDictionaryIterator;
typedef DictionaryObserver<CParameter>          ParameterDictionaryObserver;

typedef CDictionary<CSpectrum*>                 SpectrumDictionary;
typedef SpectrumDictionary::DictionaryIterator  SpectrumDictionaryIterator;
typedef DictionaryObserver<CSpectrum*>          SpectrumDictionaryObserver;

typedef CDictionary<CGateContainer>             CGateDictionary;
typedef CGateDictionary::DictionaryIterator     CGateDictionaryIterator;
typedef DictionaryObserver<CGateContainer>      GateDictionaryObserver;

/*!
   Abstract base class for gate observers:
*/
class CGateObserver : public GateDictionaryObserver {
public:
  virtual void onChange(std::string name, CGateContainer& gateContainer) = 0;
};



class CHistogrammer : public CEventSink {

  typedef std::list<CGateObserver*>   GateObserverList;

  CDisplayInterface*  m_pDisplayer;          // Points to displayer object.
  ParameterDictionary m_ParameterDictionary; // Dictionary of parameters.
  SpectrumDictionary  m_SpectrumDictionary;  // Dictionary of Spectra.
  CGateDictionary     m_GateDictionary;      // Dictionary of Gates.
  CHistogrammerFitObserver* m_pFitObserver; // Monitor for fit changes.
  GateObserverList   m_gateObservers; 


  static int          m_nextFitlineId;       // Next Xamine fitline id.

  // For maintaining the flattened lists.

  CFlattenedGateList*   m_pGateList;
  CSpectrumByParameter* m_pSpectrumLists;

 public:
  // Constructors.
  CHistogrammer(CDisplayInterface* rDisplayer);
  virtual ~CHistogrammer();
  CHistogrammer(const CHistogrammer& aCHistogrammer);

  // Operator= Assignment Operator
  CHistogrammer& operator=(const CHistogrammer& aCHistogrammer);

  // Operator== Equality Operator
  int operator== (const CHistogrammer& aCHistogrammer);
  int operator!= (const CHistogrammer& aCHistogrammer) {
    return !(operator==(aCHistogrammer));
  }

  // Selectors:
 public:


  CDisplayInterface* getDisplayer() const {
    return m_pDisplayer;
  }

  const ParameterDictionary& getParameterDictionary() const {
    return m_ParameterDictionary;
  }

  const SpectrumDictionary& getSpectrumDictionary() const {
    return m_SpectrumDictionary;
  }

  // Mutators:
 protected:

  void setDisplayer(CDisplayInterface* am_Displayer) {
    m_pDisplayer = am_Displayer;
  }

  void setParameterDictionary(const ParameterDictionary& am_ParameterDictionary) { 
    m_ParameterDictionary = am_ParameterDictionary;
  }

  void setSpectrumDictionary(const SpectrumDictionary am_SpectrumDictionary) {
    m_SpectrumDictionary = am_SpectrumDictionary;
  }

  // Operations on the object:
 public:
  // Analysis evaluation operators:
  virtual void operator() (const CEvent&     rEvent);
  virtual void operator() (CEventList& rEventList);

  // Parameter dictionary manipulation:
  CParameter* AddParameter (const std::string& sName,
			    UInt_t nId,
			    const char* pUnits);
  CParameter* AddParameter (const std::string& sName,
			    UInt_t nId,
			    UInt_t  nScale);
  CParameter* AddParameter (const std::string& sName,
			    UInt_t nId, UInt_t nScale,
			    Float_t nLow, Float_t nHi,
			    const std::string& sUnits);
  CParameter* RemoveParameter (const std::string& sName);
  CParameter* FindParameter (const std::string& rName);
  CParameter* FindParameter (UInt_t nPar);
  ParameterDictionaryIterator ParameterBegin();
  ParameterDictionaryIterator ParameterEnd();
  UInt_t ParameterCount();

  // Spectrum dictionary manipulation.
  void AddSpectrum(CSpectrum& rSpectrum);
  CSpectrum* RemoveSpectrum(const std::string sName);
  void ClearSpectrum(const std::string& rsName);
  void ClearAllSpectra();
  CSpectrum* FindSpectrum(const std::string& rName);
  CSpectrum* FindSpectrum(UInt_t id);
  SpectrumDictionaryIterator SpectrumBegin();
  SpectrumDictionaryIterator SpectrumEnd();
  UInt_t SpectrumCount();
  void addSpectrumDictionaryObserver(SpectrumDictionaryObserver* observer);
  void removeSpectrumDictionaryObserver(SpectrumDictionaryObserver* observer);

  void UnGate(const std::string& rSpectrum); // Remove gate from spectrum

  // Manipulate the gate dictionary:
  void AddGate(const std::string& rName, UInt_t nId, CGate& rGate);
  void DeleteGate(const std::string& rGateName);
  void ReplaceGate(const std::string& rGateName, CGate& rGate);
  void ApplyGate(const std::string& rGateName,  const std::string& rSpectrum);
  CGateContainer* FindGate(const std::string& rGate);
  CGateContainer* FindGate(UInt_t nId);
  CGateDictionaryIterator GateBegin();
  CGateDictionaryIterator GateEnd();
  UInt_t GateCount();

  void addGateObserver(CGateObserver* observer);
  void removeGateObserver(CGateObserver* observer);
  
  // Utility Functions:
 protected:
  std::string createTrialTitle(std::string type, 
			       std::vector<std::string>      axes,
			       std::vector<std::string>      parameters,
			       std::vector<std::string>      yparameters,
			       std::string                   gate);
  std::string createTitle(CSpectrum* pSpectrum, UInt_t     maxLength);

  void invokeGateChangedObservers(std::string name, CGateContainer& gate);
  void createListObservers();
  bool flip2dGatePoints(CSpectrum* pSpectrum, UInt_t gXparam);
  
  
		
};

#endif
