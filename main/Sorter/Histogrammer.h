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

#ifndef __XAMINEPLUS_H
#include "Xamineplus.h"
#define __XAMINEPLUS_H
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
  virtual void onChange(STD(string) name, CGateContainer& gateContainer) = 0;
};


// Display binding management types:
typedef STD(vector)<STD(string)>                     DisplayBindings;
typedef DisplayBindings::iterator               DisplayBindingsIterator;

class CHistogrammer : public CEventSink {
  typedef STD(pair)<int, STD(string)> BoundFitline;
  typedef STD(list)<BoundFitline>     FitlineList;
  typedef STD(vector)<FitlineList>    FitlineBindings;

  typedef STD(list)<CGateObserver*>   GateObserverList;

  CXamine*            m_pDisplayer;          // Points to displayer object.
  DisplayBindings     m_DisplayBindings;     // Display id to spectrum name map.
  FitlineBindings     m_FitlineBindings;     // Fitlines bound to displayer.
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
  CHistogrammer(UInt_t nSpecbytes = knDefaultSpectrumSize);
  CHistogrammer(const CXamine& rDisplayer);
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
  const DisplayBindings&  getDisplayBindings() const {
    return m_DisplayBindings;
  }

  CXamine* getDisplayer() const {
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
  void setDisplayBindings (const DisplayBindings& am_DisplayBindings) { 
    m_DisplayBindings = am_DisplayBindings;
  }

  void setDisplayer(CXamine* am_Displayer) {
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
  CParameter* AddParameter (const STD(string)& sName,
			    UInt_t nId,
			    const char* pUnits);
  CParameter* AddParameter (const STD(string)& sName,
			    UInt_t nId,
			    UInt_t  nScale);
  CParameter* AddParameter (const STD(string)& sName,
			    UInt_t nId, UInt_t nScale,
			    Float_t nLow, Float_t nHi,
			    const STD(string)& sUnits);
  CParameter* RemoveParameter (const STD(string)& sName);
  CParameter* FindParameter (const STD(string)& rName);
  CParameter* FindParameter (UInt_t nPar);
  ParameterDictionaryIterator ParameterBegin();
  ParameterDictionaryIterator ParameterEnd();
  UInt_t ParameterCount();

  // Spectrum dictionary manipulation.
  void AddSpectrum(CSpectrum& rSpectrum);
  CSpectrum* RemoveSpectrum(const STD(string) sName);
  void ClearSpectrum(const STD(string)& rsName);
  void ClearAllSpectra();
  CSpectrum* FindSpectrum(const STD(string)& rName);
  CSpectrum* FindSpectrum(UInt_t id);
  SpectrumDictionaryIterator SpectrumBegin();
  SpectrumDictionaryIterator SpectrumEnd();
  UInt_t SpectrumCount();
  void addSpectrumDictionaryObserver(SpectrumDictionaryObserver* observer);
  void removeSpectrumDictionaryObserver(SpectrumDictionaryObserver* observer);

  void UnGate(const STD(string)& rSpectrum); // Remove gate from spectrum

  // Manipulate display bindings:
  UInt_t BindToDisplay (const STD(string)& rsName);
  void UnBindFromDisplay (UInt_t nSpec);
  DisplayBindingsIterator DisplayBindingsBegin();
  DisplayBindingsIterator DisplayBindingsEnd();
  Int_t  findDisplayBinding(STD(string) name);
  UInt_t DisplayBindingsSize();
  CSpectrum* DisplayBinding(UInt_t xid);

  // Manipulate the gate dictionary:
  void AddGate(const STD(string)& rName, UInt_t nId, CGate& rGate);
  void DeleteGate(const STD(string)& rGateName);
  void ReplaceGate(const STD(string)& rGateName, CGate& rGate);
  void ApplyGate(const STD(string)& rGateName,  const STD(string)& rSpectrum);
  CGateContainer* FindGate(const STD(string)& rGate);
  CGateContainer* FindGate(UInt_t nId);
  CGateDictionaryIterator GateBegin();
  CGateDictionaryIterator GateEnd();
  UInt_t GateCount();

  void addGateObserver(CGateObserver* observer);
  void removeGateObserver(CGateObserver* observer);

  // Manipulate the set of fits bound to Xamine:

  void addFit(CSpectrumFit& fit);
  void deleteFit(CSpectrumFit& fit);

  // Utility Functions:
 protected:
  CDisplayGate* GateToXamineGate(UInt_t nBindingId, CGateContainer& rGate);
  STD(vector)<CGateContainer> GatesToDisplay(const STD(string)& rSpectrum);
  void AddGateToBoundSpectra(CGateContainer& rGate);
  void RemoveGateFromBoundSpectra(CGateContainer& rGate);
  STD(string) createTrialTitle(STD(string) type, 
			       STD(vector)<STD(string)>      axes,
			       STD(vector)<STD(string)>      parameters,
			       STD(vector)<STD(string)>      yparameters,
			       STD(string)                   gate);
  STD(string) createTitle(CSpectrum* pSpectrum, UInt_t     maxLength);

  void invokeGateChangedObservers(STD(string) name, CGateContainer& gate);
  void createListObservers();
  bool flip2dGatePoints(CSpectrum* pSpectrum, UInt_t gXparam);
		
};

#endif
