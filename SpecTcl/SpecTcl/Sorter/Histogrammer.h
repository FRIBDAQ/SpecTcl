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
                               

#ifndef __XAMINEPLUS_H
#include "Xamineplus.h"
#endif

#ifndef __DICTIONARY_H
#include "Dictionary.h"
#endif

#ifndef __PARAMETER_H
#include "Parameter.h"
#endif

#ifndef __SPECTRUM_H
#include "Spectrum.h"
#endif

#ifndef __EVENTSINK_H
#include "EventSink.h"
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __GATECONTAINER_H
#include <GateContainer.h>
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif
      
//
// Typedefs for some of instances of templated classes:
//

// Dictionary types:

typedef CDictionary<CParameter>                  ParameterDictionary;
typedef ParameterDictionary::DictionaryIterator  ParameterDictionaryIterator;

typedef CDictionary<CSpectrum*>                 SpectrumDictionary;
typedef SpectrumDictionary::DictionaryIterator  SpectrumDictionaryIterator;

typedef CDictionary<CGateContainer>             CGateDictionary;
typedef CGateDictionary::DictionaryIterator     CGateDictionaryIterator;

// Display binding management types:

typedef vector<std::string>                     DisplayBindings;
typedef DisplayBindings::iterator               DisplayBindingsIterator;



//
                                                         
class CHistogrammer : public CEventSink      
{
  CXamine*              m_pDisplayer;      // Points to displayer object.
  DisplayBindings       m_DisplayBindings; // Display id to spectrum name map
  ParameterDictionary   m_ParameterDictionary; // Dictionary of parameters.
  SpectrumDictionary    m_SpectrumDictionary;  // Dictionary of Spectra.
  CGateDictionary       m_GateDictionary; //      Dictionary of Gates. 
  
public:
  // Constructors:

  CHistogrammer (UInt_t nSpecbytes = knDefaultSpectrumSize);
  CHistogrammer (const CXamine&      rDisplayer);
  virtual ~ CHistogrammer ( );
	
			//Copy constructor

  CHistogrammer (const CHistogrammer& aCHistogrammer );

			//Operator= Assignment Operator

  CHistogrammer& operator= (const CHistogrammer& aCHistogrammer);

			//Operator== Equality Operator

  int operator== (const CHistogrammer& aCHistogrammer);
  int operator!= (const CHistogrammer& aCHistogrammer) {
    return !(operator==(aCHistogrammer)); 
  }

  // Selectors:

public:

  const DisplayBindings&  getDisplayBindings() const
  {
    return m_DisplayBindings;
  }
  CXamine* getDisplayer() const
  {
    return m_pDisplayer;
  }
  const ParameterDictionary& getParameterDictionary() const
  {
    return m_ParameterDictionary;
  }
  const SpectrumDictionary& getSpectrumDictionary() const
  {
    return m_SpectrumDictionary;
  }
  // Mutators

protected:

  void setDisplayBindings (const DisplayBindings& am_DisplayBindings)
  { 
    m_DisplayBindings = am_DisplayBindings;
  }
  void setDisplayer (CXamine* am_Displayer)
  { 
    m_pDisplayer = am_Displayer;
  }
  void setParameterDictionary (
			   const ParameterDictionary& am_ParameterDictionary)
  { 
    m_ParameterDictionary = am_ParameterDictionary;
  }
  void setSpectrumDictionary (const SpectrumDictionary am_SpectrumDictionary)
  { 
    m_SpectrumDictionary = am_SpectrumDictionary;
  }
  //
  // Operations on the object:
  //
public:
  // Analysis evaluation operators:

  virtual void operator() (const CEvent&     rEvent) ;
  virtual void operator() (CEventList& rEventList);
  
  // Parameter dictionary manipulation:

  CParameter* AddParameter (const std::string& sName,
			    UInt_t nId, 
			    UInt_t  nScale)  ;
  CParameter* RemoveParameter (const std::string& sName)  ;
  CParameter* FindParameter (const std::string& rName)  ;
  CParameter* FindParameter (UInt_t nPar);
  ParameterDictionaryIterator 
              ParameterBegin ()  ;
  ParameterDictionaryIterator 
              ParameterEnd ()  ;
  UInt_t      ParameterCount ()  ;

  // Spectrum dictionary manipulation.

  void        AddSpectrum (CSpectrum& rSpectrum)  ;
  CSpectrum*  RemoveSpectrum (const std::string sName)  ;
  void        ClearSpectrum (const std::string& rsName)  ;
  void        ClearAllSpectra ()  ;
  CSpectrum*  FindSpectrum (const std::string& rName)  ;
  CSpectrum*  FindSpectrum (UInt_t id);
  SpectrumDictionaryIterator 
              SpectrumBegin ()  ;
  SpectrumDictionaryIterator 
              SpectrumEnd ()  ;
  UInt_t      SpectrumCount ()  ;

  void UnGate(const std::string& rSpectrum); // Remove gate from spectrum

  // Manipulate display bindings:

  UInt_t      BindToDisplay (const std::string& rsName)  ;
  void        UnBindFromDisplay (UInt_t nSpec)  ;
  DisplayBindingsIterator 
              DisplayBindingsBegin ()  ;
  DisplayBindingsIterator 
              DisplayBindingsEnd ()  ;
  UInt_t      DisplayBindingsSize ()  ;
  CSpectrum*  DisplayBinding(UInt_t xid);

  // Manipulate the gate dictionary:
 
  void AddGate(const std::string& rName, UInt_t nId, CGate& rGate);
  void DeleteGate(const std::string& rGateName);
  void ReplaceGate(const std::string& rGateName, CGate& rGate);
  void ApplyGate(const std::string& rGateName,  const std::string& rSpectrum);
  CGateContainer* FindGate(const std::string& rGate);
  CGateContainer* FindGate(UInt_t nId);
  CGateDictionaryIterator GateBegin();
  CGateDictionaryIterator GateEnd();
  UInt_t                  GateCount();

  // Utility Functions:
protected:
  CDisplayGate* GateToXamineGate(UInt_t nBindingId, CGateContainer& rGate);
  std::vector<CGateContainer> GatesToDisplay(const std::string& rSpectrum);
  void AddGateToBoundSpectra(CGateContainer& rGate);
  void RemoveGateFromBoundSpectra(CGateContainer& rGate);
};

#endif
