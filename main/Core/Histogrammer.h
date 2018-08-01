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

#ifndef HISTOGRAMMER_H  //Required for current class
#define HISTOGRAMMER_H

#include <string>
#include <vector>
#include <list>
#include <histotypes.h>
#include "Parameter.h"
#include "Spectrum.h"
#include "Dictionary.h"
#include "EventSink.h"
#include <GateContainer.h>
#include <GatingObserver.h>

// Forward class definitions (probably should be a lot more of these).

class CFlattenedGateList;
class CSpectrumByParameter;
class CDisplayGate;

// Typedefs for some of instances of templated classes:
// Dictionary types:
typedef CDictionary<CParameter>                 ParameterDictionary;
typedef ParameterDictionary::DictionaryIterator ParameterDictionaryIterator;
typedef DictionaryObserver<CParameter>          ParameterDictionaryObserver;

typedef CDictionary<CSpectrum*>                 SpectrumDictionary;
typedef SpectrumDictionary::DictionaryIterator  SpectrumDictionaryIterator;
typedef SpectrumDictionary::ConstDictionaryIterator  ConstSpectrumDictIterator;
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


/*!
 * \brief The CHistogrammer class
 *
 * TIP: This is often referred to as the sorter.
 *
 * This class provides the logic for filling the spectra when an event is complete.
 * It also owns all of the dictionaries for the spectra, gates, and parameters.
 * It also manages the observers for the gate dictionary.
 *
 * \todo Factor out the dictionaries from this.
 */
class CHistogrammer : public CEventSink {

  typedef std::list<CGateObserver*>   GateObserverList;
  typedef std::list<CGatingObserver*> GatingObserverList;


  ParameterDictionary m_ParameterDictionary; // Dictionary of parameters.
  
  
  SpectrumDictionary  m_SpectrumDictionary;  // Dictionary of Spectra.
  CGateDictionary     m_GateDictionary;      // Dictionary of Gates.
  GateObserverList    m_gateObservers;       // Observers of gate dict
  GatingObserverList  m_gatingObservers;     // Observers of applyGate/ungate

  
  
  // For maintaining the flattened lists.

  CFlattenedGateList*   m_pGateList;
  CSpectrumByParameter* m_pSpectrumLists;

 public:
  // Constructors.
  CHistogrammer();
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

  const ParameterDictionary& getParameterDictionary() const {
    return m_ParameterDictionary;
  }

  const SpectrumDictionary& getSpectrumDictionary() const {
    return m_SpectrumDictionary;
  }

  // Mutators:
 protected:

  void setParameterDictionary(const ParameterDictionary& am_ParameterDictionary) { 
    m_ParameterDictionary = am_ParameterDictionary;
  }

  void setSpectrumDictionary(const SpectrumDictionary& am_SpectrumDictionary) {
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

  void addGatingObserver(CGatingObserver* observer);
  void removeGatingObserver(CGatingObserver* observer);
  
  void addParameterObserver(ParameterDictionaryObserver* pObserver);
  void removeParameterObserver(ParameterDictionaryObserver* pObsevrer);
  

  
  // Utility Functions:
 protected:

  void invokeGateChangedObservers(std::string name, CGateContainer& gate);
  void createListObservers();
  
  void observeApplyGate(CGateContainer& rGate, CSpectrum& rSpectrum);
		
};

#endif
