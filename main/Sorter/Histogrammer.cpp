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


#include <config.h>

// Header Files:
#include "Histogrammer.h"    
#include <Exception.h>                           
#include "DictionaryException.h"
#include "EventList.h"

#include <Display.h>
#include <Xamineplus.h>
#include <Xamine1D.h>
#include <Xamine2D.h>
#ifdef USE_MAPPED_SPECTRA
#include <XamineMap1D.h>
#include <XamineMap2D.h>
#endif
#include <XamineGates.h>
#include <Xamine.h>
#include <DisplayGate.h>
#include <TrueGate.h>
#include <FalseGate.h>
#include <DeletedGate.h>
#include <Cut.h>
#include <Band.h>
#include <Contour.h>
#include <Point.h>
#include <PointlistGate.h>
#include <CGammaCut.h>
#include <CGammaBand.h>
#include <CGammaContour.h>
#include <assert.h>
#include <GateMediator.h>
#include <Gamma2DW.h>
#include <CSpectrumFit.h>
#include "CHistogrammerFitObserver.h"
#include <CFitDictionary.h>
#include <CFlattenedGateList.h>
#include <CSpectrumByParameter.h>
#include <DisplayInterface.h>


#include <iostream>
#include <sstream>

#include <stdio.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

#ifdef HAVE_TIME
#include <time.h>
#endif

//
//   Local variables:
// 

static string U("-Ungated-");
static string D("-Deleted-");
static CTrueGate   AlwaysTrue;
static CDeletedGate  AlwaysFalse;
static CGateContainer NoGate(U, 0,      AlwaysTrue);
static CGateContainer Deleted(D, 0, AlwaysFalse);


// Static member data:

int CHistogrammer::m_nextFitlineId(1); // Next fitline id assigned.

// Very stupid local function to do parameter scaling: 
static inline UInt_t scale(UInt_t nValue, Int_t nScale) {
  return ((nScale > 0) ? (nValue >> nScale) :
	  (nValue << (-nScale)));
}

// Helper classes: These are predicates which are used
// to match partial spectrum and parameter names.
class SpectrumPartialMatchPredicate {
  std::string m_Name;
public:
  SpectrumPartialMatchPredicate(const std::string& rName) :
    m_Name(rName) {}
  Bool_t operator()(pair<const std::string, CSpectrum*> i) {
    CSpectrum* pSpectrum = i.second;
    return ( (pSpectrum->getName().find(m_Name.c_str())) == 0);
  }
};

// Predicate to doa partial (first part) match of a stored string
// with a candidate string.
class ParameterPartialMatchPredicate {
  std::string m_Name;
public:
  ParameterPartialMatchPredicate(const std::string& rName) :
    m_Name(rName) {}
  Bool_t operator()(pair<const std::string, CParameter> i){
    CParameter p = i.second;
    std::string name = p.getName();
    return (name.find(m_Name.c_str()) == 0);
  }
};

// Predicate to match a stored Id against a candidate Id (in traversing
// the bind list by id e.g.).
class IdMatchPredicate {
  UInt_t m_nId;
public:
  IdMatchPredicate(UInt_t nId) : m_nId(nId) {}
  Bool_t operator()(pair<const std::string, CNamedItem> i) {
    return (i.second.getNumber() == m_nId);
  }
};

// Same as above, but instead of the id we get a pointer to a named object
// as the parameter.
class PointerIdMatchPredicate {
  UInt_t m_nId;
public:
  PointerIdMatchPredicate(UInt_t nId) : m_nId(nId) {}
  Bool_t operator()(pair<const std::string, CNamedItem*> i) {
    return ((i.second)->getNumber() == m_nId);
  }
};

// Functions for class CHistogrammer

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//     CHistogrammer(UInt_t nSpecbytes)
//  Operation Type:
//     Constructor.
//
/*!
  Constructor:

  Set the numer of bytes of spectrum memory to request.
  In addition, the randomizer is seeded here (it's a convenient place),
  and the displayer is started.

  \param <TT>nSpecBytes (UInt_t [in]):</TT>
  Number of bytes of displayer memory to reserve.

*/
CHistogrammer::CHistogrammer(UInt_t nSpecbytes) :
  m_pDisplayer(0)
{
  srand(time(NULL));
  m_pDisplayer = new CXamine(nSpecbytes);
  m_pDisplayer->Start();
  m_pFitObserver = new CHistogrammerFitObserver(*this); // Follow changes to fits.

  // Create and register the gate/histogram observer needed to maitain the gate/spectrum
  // optimized lists:

  createListObservers();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CHistogrammer(const CDisplayInterface&  rDisplayer)
//  Operation Type:
//    Construtor
//

CHistogrammer::CHistogrammer(const CDisplay &rDisplayer) :
    m_pDisplayer(rDisplayer.clone())
{
  srand(time(NULL)); 
  if(!m_pDisplayer->isAlive())
    m_pDisplayer->Start();
  m_pFitObserver = new CHistogrammerFitObserver(*this); // Follow changes to fits.
  createListObservers();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    ~CHistogrammer()
//  Operation Type:
//     Destructor.
//
CHistogrammer::~CHistogrammer() {
  if(m_pDisplayer->isAlive())
    m_pDisplayer->Stop();

  delete m_pDisplayer;
  delete m_pFitObserver;
  delete m_pGateList;
  delete m_pSpectrumLists;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CHistogrammer(const CHistogrammer& rRhs)
// Operation Type:
//    Copy constructor
//
CHistogrammer::CHistogrammer(const CHistogrammer& rRhs) :
    m_pDisplayer(rRhs.m_pDisplayer->clone())
{
  m_DisplayBindings     = rRhs.m_DisplayBindings;
  m_ParameterDictionary = rRhs.m_ParameterDictionary;
  m_SpectrumDictionary  = rRhs.m_SpectrumDictionary;
  m_GateDictionary      = rRhs.m_GateDictionary;

  createListObservers();
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CHistogrammer& operator=(const CHistogrammer& rRhs)
//
// Operation Type:
//    Assignment operator.
// 
CHistogrammer& CHistogrammer::operator=(const CHistogrammer& rRhs) {
  m_pDisplayer          = rRhs.m_pDisplayer->clone();
  m_DisplayBindings     = rRhs.m_DisplayBindings;
  m_ParameterDictionary = rRhs.m_ParameterDictionary;
  m_SpectrumDictionary  = rRhs.m_SpectrumDictionary;
  m_GateDictionary      = rRhs.m_GateDictionary;

  createListObservers();
  return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//   int operator==(const CHistogrammer& rRhs)
// Operation Type:
//   Equality comparison.
//
int CHistogrammer::operator==(const CHistogrammer& rRhs) {
  return (
	  (*m_pDisplayer          == *(rRhs.m_pDisplayer))         &&
	  ( m_DisplayBindings     ==   rRhs.m_DisplayBindings)     &&
	  ( m_ParameterDictionary ==   rRhs.m_ParameterDictionary) &&
	  ( m_SpectrumDictionary  ==   rRhs.m_SpectrumDictionary)  &&
	  ( m_GateDictionary      ==   rRhs.m_GateDictionary)
	  );
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    void operator() (const CEvent& rEvent)
// Operation Type:
//    Analyzer function
//
/*! 
  Histograms a single event by passig it to all the histograms.
  the histograms take the parameters they need from the event and
  histogram themselves.

  In order to avoid the need to traverse maps, flattened versions of the
  spectrum and gate dictionaroes are made.  The nSpectra,ppSpectra and
  nGates,ppGates parameters specify these traversal optimized structures
  (simple arrays).
   
  \par Formal Parameters:
  \param  </TT>rEvent (const CEvent& [in]): </TT>
  References the event to histogram.

*/
void CHistogrammer::operator()(const CEvent& rEvent)
{
  // Reset the gates:

  CGateContainer** gateList = m_pGateList->getList();
  if (gateList) {
    while(*gateList) {
      (**gateList)->Reset();
      gateList++;
    }
  }

  // Increment the histograms:

  (*m_pSpectrumLists)(rEvent);

}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//     void operator()(CEventList& rEvents)
// Operation Type:
//     Functionalizer.
//
static  int nEvents = 0;

void CHistogrammer::operator()(CEventList& rEvents) 
{

  nEvents = 0;


  //  Now analyze the events.
  //  the assumption is that the first null
  //  event or the end of the vector terminates.
  //
  try {
    CEventListIterator i;
    CEventListIterator e = rEvents.end();
    for(i = rEvents.begin(); i != e; i++) {
      CEvent* pEvent = *i;
      if(pEvent) {
	nEvents++;
	operator()(*pEvent);
      }
      else {
	break;
      }
    }
  }
  catch (CException& e) {
    cerr << "Exception caught while histogramming events: "
	 << e.ReasonText() << " while " << e.WasDoing() <<endl;
  }
  catch (string msg) {
    cerr << "String exception caught while histogramming events: "
	 << msg << endl;
  }
  catch (const char* msg) {
    cerr << "Char* exception caught while histogramming events: "
	 << msg <<endl;
  }
  catch (...) {
    cerr << "Unexpected exception type caught while histogramming events.\n";
  }

}

/*!
  Add a parameter to the parameter dictionary.
  This overloaded function adds a Real (unscaled) parameter
  that has associated optional units.  
  \param sName (const std::string& [in]): Name of the parameter must be
  unique or we will throw a dictionary exception for a duplicate key.
  \param nId   (UInt_t [in]): Id of the parameter.  Must be unique or
  we will throw a dictionary exception for a duplicate id.
  \param pUnits (const char* [in]): If not null, this points to the units
  text for the parameter.
  \return A pointer to the created parameter.
  \throw CDictionaryException for various reasons outlined above.
*/
CParameter* CHistogrammer::AddParameter(const std::string& sName,
					UInt_t nId, const char* pUnits)
{
  // Avoid Duplication:
  CParameter* pPar = FindParameter(sName);
  if(pPar) 
    if(pPar->getName() == sName) { // Duplicate parameter name.
      throw CDictionaryException(CDictionaryException::knDuplicateKey,
				 "CHistogrammer::AddParameter Checking for duplicate parameter name",
				 sName);
    }
  pPar = FindParameter(nId);
  if(pPar) {			// Duplicate parameter id.
    throw CDictionaryException(CDictionaryException::knDuplicateId,
			       "CHistogrammer::AddParameter Checking for duplicate parameter id",
			       nId);
  }

  // Now add the new parameter to the dictionary:

  CParameter par(sName, nId, pUnits);

  m_ParameterDictionary.Enter(string(sName), 
			      par);
  return FindParameter(sName);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CParameter* AddParameter ( const std::string& sName, 
//                               UInt_t nId, UInt_t  nScale )
//  Operation Type:
//    Mutator
//
CParameter* CHistogrammer::AddParameter(const std::string& sName, UInt_t nId,
					UInt_t  nScale) 
{
  // Adds a parameter to the parameter dictionary.
  // Parameters have names, numbers and ranges.
  // The name and number must be unique, and this 
  // uniqueness is enforced by the dictionary.
  //
  // Formal Parameters:
  //          const std::string&  sName:
  //                  Name of the parameter to add
  //          UInt_t nId:
  //                   Number of the parameter to add.
  //          UInt_t nScale:
  //                   Log[2] of the parameter full scale.
  //
  // Throws:
  //     CDictionaryException if parameter with same name or ID already
  //     exists.
  // Returns:
  //     Pointer to the parameter created, kpNULL If unable to enter.

  // Avoid Duplication:
  CParameter* pPar = FindParameter(sName);
  if(pPar) 
    if(pPar->getName() == sName) { // Duplicate parameter name.
      throw CDictionaryException(CDictionaryException::knDuplicateKey,
				 "CHistogrammer::AddParameter Checking for duplicate parameter name",
				 sName);
    }
  pPar = FindParameter(nId);
  if(pPar) {			// Duplicate parameter id.
    throw CDictionaryException(CDictionaryException::knDuplicateId,
			       "CHistogrammer::AddParameter Checking for duplicate parameter id",
			       nId);
  }

  // Now add the new parameter to the dictionary:

  CParameter par(nScale, sName, nId);
  m_ParameterDictionary.Enter(string(sName), par);
  return FindParameter(sName);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CParameter* AddParameter ( const std::string& sName, 
//                               UInt_t nId, UInt_t  nScale,
//                               Float_t nLow, Float_t nHi, 
//                               const std::string& sUnits=string("") )
//  Operation Type:
//    Mutator
//
CParameter* CHistogrammer::AddParameter(const std::string& sName, UInt_t nId,
					UInt_t  nScale, Float_t nLow, Float_t nHi,
					const std::string& sUnits)
{
  // Adds a parameter to the parameter dictionary.
  // Parameters have names, numbers and ranges.
  // The name and number must be unique, and this 
  // uniqueness is enforced by the dictionary.
  //
  // Formal Parameters:
  //          const std::string&  sName:
  //                  Name of the parameter to add
  //          UInt_t nId:
  //                   Number of the parameter to add.
  //          UInt_t nScale:
  //                   Log[2] of the parameter full scale.
  //
  // Throws:
  //     CDictionaryException if parameter with same name or ID already
  //     exists.
  // Returns:
  //     Pointer to the parameter created, kpNULL If unable to enter.

  // Avoid Duplication:
  CParameter* pPar = FindParameter(sName);
  if(pPar) 
    if(pPar->getName() == sName) { // Duplicate parameter name.
      throw CDictionaryException(CDictionaryException::knDuplicateKey,
				 "CHistogrammer::AddParameter Checking for duplicate parameter name",
				 sName);
    }
  pPar = FindParameter(nId);
  if(pPar) {			// Duplicate parameter id.
    throw CDictionaryException(CDictionaryException::knDuplicateId,
			       "CHistogrammer::AddParameter Checking for duplicate parameter id",
			       nId);
  }

  // Now add the new parameter to the dictionary:

  CParameter par(nScale, sName, nId, nLow, nHi, sUnits);
  m_ParameterDictionary.Enter(string(sName), par);
					 
  return FindParameter(sName);  
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void AddSpectrum ( CSpectrum& rSpectrum )
//  Operation Type:
//     mutator.
//
void CHistogrammer::AddSpectrum(CSpectrum& rSpectrum) {
  // Adds a spectrum to the parameter histogrammer's spectrum dictionary.
  // Since in theory a whole variety of histogram types will be definable
  // in the future, the histogram must be created externally and passed in.
  // Histogram names must be unique and their numbers must also
  // be unique.  This uniqueness will be enforced by the histogram
  // dictionary.
  //  
  // Formal Parameters:
  //      CSpectrum& rSpec:
  //          Refers to a spectrum to add to the system.
  //          The spectrum should be dynamically allocated.
  //          Ownership of the spectrum object passes to the
  //          CHistogrammer, which will delete it at destruction.
  //          On Removal, the histogrammer will pass control
  //          of the histogram back to the caller.
  //  
  // Exceptions: 
  //   CDictionaryException If a spectrum with that name and that
  //   id already exist.
  // 

  // Prevent duplicate spectrum at cost of a CDictionaryException.

  CSpectrum* pSpec = FindSpectrum(rSpectrum.getName());
  if(pSpec) {
    if(pSpec->getName() == rSpectrum.getName()) { // If exact match freak.
      throw CDictionaryException(CDictionaryException::knDuplicateKey,
				 "CHistogrammer::AddHistogram Checking for duplicate spectrum name",
				 rSpectrum.getName());
    }
  }
  pSpec = FindSpectrum(rSpectrum.getNumber());
  if(pSpec) {
    throw CDictionaryException(CDictionaryException::knDuplicateKey,
			       "CHistogrammer::AddHistogram Checking for duplicate spectrum id",
			       rSpectrum.getNumber());
  }
  //
  // Now enter the item:
  //
  CSpectrum*  pSpectrum = &rSpectrum;
  m_SpectrumDictionary.Enter(rSpectrum.getName(), pSpectrum);

}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CParameter* RemoveParameter ( const std::string& sName )
//  Operation Type:
//     Mutator
//
CParameter* CHistogrammer::RemoveParameter(const std::string& sName) {
  // Removes the named parameter from the histogrammer.
  // Ownership of the parameter passes to the remover.
  // Note that the returned parameter is dynamically allocated.
  // Therefore the caller should delete the object at some
  //  point.
  //
  // Formal Parameters:
  //     const std::string& sName:
  //              Name of the parameter to remove (must match exactly).
  // Returns:
  //    CParameter* - pointer to the parameter to remove or
  //    kpNULL if there isn't any such parameter.

  ParameterDictionaryIterator iPar = m_ParameterDictionary.Lookup(sName);
  CParameter*                  pPar((CParameter*)kpNULL);
  if(iPar != m_ParameterDictionary.end()) {
    pPar = new CParameter((*iPar).second); // Duplicate  dynamically.
    m_ParameterDictionary.Remove(sName);
  }
  return pPar;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CSpectrum* RemoveSpectrum ( const std::string sName )
//  Operation Type:
//     Mutator
//
CSpectrum* CHistogrammer::RemoveSpectrum(const std::string sName) {
  // Removes a spectrum from the histogrammer's spectrum
  // dictionary.  Ownership of the spectrum is transferred to 
  // the caller. The returned  spectrum
  // should be deleted at some point by the caller.
  //
  // Formal Parameters:
  //     const std::string& sName:
  //         Name of the spectrum.
  //
  //  Returns:
  //     CSpectrum*  - pointer to the spectrum removed or
  //                            kpNULL if there wasn't one by that name.

  // Ensure the spectrum exists and get a pointer to it:
  SpectrumDictionaryIterator iSpectrum = m_SpectrumDictionary.Lookup(sName);
  CSpectrum*                 pSpectrum((CSpectrum*)kpNULL);
  if(iSpectrum != m_SpectrumDictionary.end()) {
    pSpectrum = (*iSpectrum).second;

    // I don't like doing this here but I'm really not sure where else to do
    // it.. in an ideal world I'd have observers on the spectrum dictionary
    // and the deletion of a spectrum would trigger the deletion of
    // the corresponding fits... however that sort of internal restruturing
    // is a 4.0 thing.

    CFitDictionary&          Fits(CFitDictionary::getInstance());
    CFitDictionary::iterator iFit  = Fits.begin();
    while (iFit != Fits.end()) {
      CSpectrumFit* pFit = iFit->second;
      if (pFit->getName() == sName) {

	Fits.erase(iFit);	// This will also trigger remove from Xamine.
	delete pFit;

	// The iterator has been invalidated potentially so start again:

	iFit = Fits.begin();
      } 
      else {
	iFit++;
      }
    }


    // Kill off the spectrum from the dictionary.

    m_SpectrumDictionary.Remove(sName);
  }
  return pSpectrum;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void ClearSpectrum ( const std::string& rsName )
//  Operation Type:
//     mutator
//
void CHistogrammer::ClearSpectrum(const std::string& rsName) {
  // Clears the selected spectrum.
  //
  //  Formal Parameters:
  //        const std::string& rsName:
  //               Name of the spectrum to clear.


  SpectrumDictionaryIterator i = m_SpectrumDictionary.Lookup(rsName);
  if(i == m_SpectrumDictionary.end()) {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
			       "CHistogrammer::ClearSpectrum - Looking up spectrum to clear",
			       rsName);
  }
  ((*i).second)->Clear();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void ClearAllSpectra (  )
//  Operation Type:
//     mutator.
//
void CHistogrammer::ClearAllSpectra() {
  // Clears all spectra in the histogrammer's spectrum
  // dictionary.

  for(SpectrumDictionaryIterator i = m_SpectrumDictionary.begin();
      i != m_SpectrumDictionary.end(); i++) {
    ((*i).second)->Clear();
  }

}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CSpectrum* FindSpectrum ( const std::string& rName )
//  Operation Type:
//     Selector
//
CSpectrum* CHistogrammer::FindSpectrum(const std::string& rName) {
  // Locates a particular spectrum.
  // There are two forms of this function:
  //
  // Formal Parameters:
  //     const std::string& rName:
  //        Specifies the name of the spectrum to locate.
  //        Only exact matches are allowed.
  //
  //  UInt_t   nID:
  //       A match is attempted by id. 
  //
  // Returns:
  //    A pointer to the found spectrum else kpNULL if there
  //  wasn't a match.
  SpectrumDictionaryIterator iSpec = m_SpectrumDictionary.Lookup(rName);
  if(iSpec != m_SpectrumDictionary.end()) {
    return (*iSpec).second;
  }
  else {
    return (CSpectrum*)kpNULL;
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//     CSpectrum* FindSpectrum(UInt_t nSpec)
//  Operation Type:
//     Selector
//
CSpectrum* CHistogrammer::FindSpectrum(UInt_t nSpec) {
  SpectrumDictionaryIterator 
    i(m_SpectrumDictionary.FindMatch(PointerIdMatchPredicate(nSpec)));

  if(i != m_SpectrumDictionary.end()) {
    return (*i).second;
  }
  else {
    return (CSpectrum*)kpNULL;
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    SpectrumDictionaryIterator SpectrumBegin (  )
//  Operation Type:
//     Selector.
//
SpectrumDictionaryIterator CHistogrammer::SpectrumBegin() {
  // Returns an iterator which corresponds to the 'first' entry in the spectrum dictionary.
  // This iterator can be used to visit all dictionary entries.
  return m_SpectrumDictionary.begin();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    SpectrumDictionaryIterator Spectrum End()
//  Operation Type:
//     Selector
//
SpectrumDictionaryIterator CHistogrammer::SpectrumEnd() {
  // Returns an iterator which refers to 'off the end' of the
  // spectrum dictionary.  This can be used to determine when
  // all dictionary entries have been visited by an incrementing 
  // iterator.
  return m_SpectrumDictionary.end();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    UInt_t SpectrumCount()
//  Operation Type:
//     selector
//
UInt_t CHistogrammer::SpectrumCount() {
  // Returns the number of spectra in the spectrum dictionary.
  return m_SpectrumDictionary.size();
}

/*!
    Add a spectrum dictionary observer so that software can monitor
    changes to the spectrum dictionary:
    \param observer : SpectrumDictionaryObserver*
       Pointer to the observer to add.
*/
void
CHistogrammer::addSpectrumDictionaryObserver(SpectrumDictionaryObserver* observer) 
{
  m_SpectrumDictionary.addObserver(observer);
}

/*!
   Remove an existing spectrum dictionary observer from the spectrum
   dictionary.  That observer object will no longer be notified of
   changes in the spectrum dictionary.
   \param observer : SpectrumDictionaryObserver* 
         Pointer to the observer to remove.
*/
void
CHistogrammer::removeSpectrumDictionaryObserver(SpectrumDictionaryObserver* observer)
{
  m_SpectrumDictionary.removeObserver(observer);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CParameter* FindParameter ( const std::string& rName )
//  Operation Type:
//     Selector.
//
CParameter* CHistogrammer::FindParameter(const std::string& rName) {
  // Locates a specified parameter searching
  // either by name or id (overloaded).
  //
  // Formal Parameters:
  //    const std::string& rName:
  //       Name of the parameter to look for.
  //       Only an exact match is attempted. 
  //
  //  Uint_t nID:
  //       Number of the parameter.
  //
  // Returns:
  //   A pointer to the found object or kpNULL if there is no match.
  ParameterDictionaryIterator iParam = m_ParameterDictionary.Lookup(rName);
  if(iParam != m_ParameterDictionary.end()) {
    return &((*iParam).second);
  }
  else 
    return (CParameter*)kpNULL;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//     CParameter*  FindParameter(UInt_t nPar)
//
// Operation Type:
//     Selector.
CParameter* CHistogrammer::FindParameter(UInt_t nPar) {
  ParameterDictionaryIterator 
    i(m_ParameterDictionary.FindMatch(IdMatchPredicate(nPar)));

  if(i != m_ParameterDictionary.end()) {
    return &((*i).second);
  }
  else {
    return (CParameter*)kpNULL;
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    ParameterDictionaryIterator ParameterBegin()
//  Operation Type:
//     Selector
//
ParameterDictionaryIterator CHistogrammer::ParameterBegin() {
  // Returns an iterator corresponding to the 
  // first entry of the parameter dictionary.  This
  // iterator can be used to visit all nodes of the
  // dictionary.
  return m_ParameterDictionary.begin();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    ParameterDictionaryIterator ParameterEnd (  )
//  Operation Type:
//     Selector
//
ParameterDictionaryIterator CHistogrammer::ParameterEnd() {
  // Returns an iterator which corresponds to just
  // off the end of the parameter dictionary. This can
  // be used to determine if an iterator has visited all
  // nodes in the dictionary.
  return m_ParameterDictionary.end();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    UInt_t ParameterCount()
//  Operation Type:
//     Selector
//
UInt_t CHistogrammer::ParameterCount() {
  // Returns the number of entries in the parameter dictionary.
  return m_ParameterDictionary.size();
}
///////////////////////////////////////////////////////////////////////////////
//
// Function:
//     void UnGate(const std::string& rSpectrum);
// Operation Type:
//     Mutator.
void CHistogrammer::UnGate(const std::string& rSpectrum) {
  // Removes a gate from the indicated spectrum.
  // Note that the gate is really replaced with a
  // pointer to the noGate container.  That container holds a pointer to
  // a CTrueGate which will always allow the spectrum to be incremented.
  //
  // Formal Parameters:
  //     const std::string& rSpectrum:
  //        Reference to the spectrum to ungate.
  // Exceptions:
  //     CDictionaryException  if the spectrum does not exist.
  //

  CSpectrum *pSpectrum = FindSpectrum(rSpectrum);
  if(!pSpectrum) {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
			       "Looking up spectrum in CHistogrammer::UnGate",
			       rSpectrum);
  }
  else {
    pSpectrum->ApplyGate(&NoGate);
    Int_t b  = findDisplayBinding(pSpectrum->getName());
    if (b >= 0) {
      m_pDisplayer->setTitle(pSpectrum->getName(), b);
      m_pDisplayer->setInfo(createTitle(pSpectrum,
					 m_pDisplayer->getTitleSize()), b);
    }
    
  }
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//       void Addgate(const std::string& rName, UInt_t nId, CGate& rGate)  
// Operation Type:
//     Mutators.
//
void CHistogrammer::AddGate(const std::string& rName, UInt_t nId, CGate& rGate) {
  // Adds a gate to the gate dictionary.  The gate must have a unique name and
  // a unique ID, or else a DictinoaryException is thrown.  The gate is 
  // cloned into a gate container and added to the gate dictinoary.
  //
  // Formal Parameters:
  //     const std::string& rName
  //        Name of new gate.
  //     UInt_t nId:
  //        Number of the gate to insert in the dictionary.
  //     CGate& rGate:
  //        refers to the gate to add.
  //
  // Exceptions:
  //    CDictionaryException if:
  //         Gate by this name already exists.
  //         Gate with this Id already exists.
  //
  CGateContainer* pGate = FindGate(rName);
  if(pGate) {			// Duplicate name...
    throw CDictionaryException(CDictionaryException::knDuplicateKey,
			       "Looking up gate name in CHistogrammer::AddGate",
			       rName);
  }
  pGate = FindGate(nId);
  if(pGate) {			// Duplicate Gate Id.
    throw CDictionaryException(CDictionaryException::knDuplicateKey,
			       "Looking up gate id. in CHistogrammer::AddGate",
			       rName);
  }

  // Now add the gate:
  CGateContainer& aGate(*(new CGateContainer((string&)rName, nId, rGate)));
  m_GateDictionary.Enter(rName, aGate);
  AddGateToBoundSpectra(aGate);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//     void DeleteGate(const std::string& rGateName)
// Operation Type:
//     Mutator
//
void CHistogrammer::DeleteGate(const std::string& rGateName) {
  // Deletes an existing gate from the gate dictionary... This really
  // just replaces the gate in the gate container with a 
  // pointer to an always false gate.
  //
  // Formal Parameters:
  //   const std::string& rGate:
  //      The Gate to 'delete'.
  //
  ReplaceGate(rGateName, AlwaysFalse);
}

/////////////////////////////////////////////////////////////////////
//
//  Function:
//     void ReplaceGate(const std::string& rGateName, CGate& rGate)
// Operation Type:
//     Mutator.
// 
void CHistogrammer::ReplaceGate(const std::string& rGateName, CGate& rGate) {
  // Replaces an existing gate with a new gate.
  // 
  // Formal Parameters:
  //     const std::string& rGateName:
  //       Name of the gate to be replaced.
  //     CGate& rGate:
  //       New gate to replace existing gate with.
  // Throws:
  //    CDictionaryException if:
  //      named Gate does not exist.
  //
  // NOTE:
  //    The gate is cloned and attached to the existing gate container.
  //

  CGateContainer* pContainer = FindGate(rGateName);
  if(!pContainer) {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
			       "Searching for existing gate in CHistogrammer::ReplaceGate",
			       rGateName);
  }
  // We must remove the old gate from the displays, replace with the
  // new gate and Add back.

  RemoveGateFromBoundSpectra(*pContainer);
  pContainer->setGate(&rGate);
  invokeGateChangedObservers(rGateName, *pContainer);
  AddGateToBoundSpectra(*pContainer);
}

/////////////////////////////////////////////////////////////////////
//
// Function:
//    void ApplyGate(const std::string& rGateName,  
//                   const std::string& rSpectrum)
// Operation Type:
//    Mutator.
void CHistogrammer::ApplyGate(const std::string& rGateName, 
			      const std::string& rSpectrum)
{
  // Applies a gate to a spectrum.  After applying the gate to the
  // spectrum, the spectrum will only be histogrammed for events where
  // the gate is 'made.'
  //
  //  Formal Parameters:
  //     const std::string& rGateName:
  //         Name of the gate to apply.
  //     const std::string& rSpectrum:
  //         Name of spectrum to which gate is applied.
  // 
  CSpectrum*      pSpectrum;
  CGateContainer* pGateContainer;

  pSpectrum = FindSpectrum(rSpectrum);
  if(!pSpectrum) {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
			       "No Such Spectrum in CHistogrammer::ApplyGate()",
			       rSpectrum);
  }

  pGateContainer = FindGate(rGateName);
  if(!pGateContainer) {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
			       "No Such Gate in CHistogrammer::ApplyGate()",
			       rGateName);
  }

  SpectrumType_t spType = pSpectrum->getSpectrumType();
  pSpectrum->ApplyGate(pGateContainer);
  Int_t b =  findDisplayBinding(rSpectrum);
  if(b >= 0) {
    m_pDisplayer->setTitle(pSpectrum->getName(), b);
    m_pDisplayer->setInfo(createTitle(pSpectrum,
				      m_pDisplayer->getTitleSize()),
			  b);
  }
}
/////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CGateContainer* FindGate(const std::string& rGate)
// Operation Type:
//    Selector.
//
CGateContainer* CHistogrammer::FindGate(const std::string& rGate) {
  CGateDictionaryIterator p = m_GateDictionary.Lookup(rGate);
  return p != m_GateDictionary.end() ? &((*p).second) : 
    (CGateContainer*)kpNULL;
}

///////////////////////////////////////////////////////////////////////////
//
// Function:
//   CGateContainer* FindGate(UInt_t nId)
// Opeation Type:
//    Selector.
//
CGateContainer* CHistogrammer::FindGate(UInt_t nId) {
  // Find a gate given the Id rather than the name.
  IdMatchPredicate Match(nId);
  CGateDictionaryIterator p(m_GateDictionary.FindMatch(Match));

  return p != m_GateDictionary.end() ? &((*p).second) : 
    (CGateContainer*)kpNULL;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//   CGateDictionaryIterator GateBegin()
// Operation Type:
//   Selector.
//
CGateDictionaryIterator CHistogrammer::GateBegin() {
  return m_GateDictionary.begin();
}

/////////////////////////////////////////////////////////////////////////////
// 
// Function:
//    CGateDictionaryIterator GateEnd()
// Operation Type:
//    Selector.
//
CGateDictionaryIterator CHistogrammer::GateEnd() {
  return m_GateDictionary.end();
}

////////////////////////////////////////////////////////////////////////////
//
// Function:
//    UInt_t GateCount()
// Operation Type:
//    Selector.
//
UInt_t CHistogrammer::GateCount() {
  return m_GateDictionary.size();
}

/*!
    Add a gate observer.  The gate observer must be added to both the
    gate dictionary and our own gate observer list as we need to trigger the
    gate changed calls ourself.
    \param observer - Pointer to the gate observer to add to the dictionary
                     as well as our own list.
*/
void
CHistogrammer::addGateObserver(CGateObserver* observer)
{
  m_GateDictionary.addObserver(observer);

  m_gateObservers.push_back(observer);
}

/*!
  Remove a gate observer.  This is a no-op if the observer pointed
  to by the parameter does not exist.

  \param observer - Pointer to tyhe observer to remove. 
*/
void
CHistogrammer::removeGateObserver(CGateObserver* observer)
{
  m_GateDictionary.removeObserver(observer);

  GateObserverList::iterator p = m_gateObservers.begin();
  while (p != m_gateObservers.end()) {
    if (*p == observer) {
      m_gateObservers.erase(p);
      return;
    }
    p++;
  }
}
///////////////////////////////////////////////////////////////////////////
//
//  Creates a trial spectrum title.  This unconditionally
//  glues the elements of a title together to form a title.
//  The string created is of the form:
//   title : type [low hi chans] x [low hi chans]  Gated on  gatename : {parameters...}
//  However:
//    - If there are no axes (size of vector is 0), the axes are omitted.
//    - If there are no characters in the gate name, the gate is omitted.
//    - If the parameters vector is size 0 it is omitted too.
//   The idea is for createTitle to use this to iteratively try to fit
//   title elements into the number of characters accepted by a displayer.
// Parameters:
//     name  : string
//          Spectrum name
//     type  : string
//          type of the spectrum.
//     axes  : vector<string>
//          Axis names.
//     parameters : vector <string>
//          Names of parameters.
//     yparameter : vector<string>
//         vector of y axis parameters (gamma 2d deluxe).
//     gate : string
//          Name of gate on spectrum.
//
// Returns:
//    A string that describes the spectrum in standard from from these
//    elements.
//
string
CHistogrammer::createTrialTitle(string type, vector<string>      axes,
				vector<string>      parameters,
				vector<string>      yparameters,
				string gate)
{
  string result;
  result += type;

  // If there are axes, put them in:

  if (axes.size() > 0) {
    string separator = " ";
    for (int i =0; i < axes.size(); i++) {
      result += separator;
      result += "[";
      result += axes[i];
      result += "]";

      separator = " X ";
    }
  }
  // If there's a nonempty gate string add that information:
  
  if (gate != string("")) {
    result += " Gated on : ";
    result += gate;
  }
  // If there are parameters comma separate them in curlies.
  
  if(parameters.size() > 0) {
    string separator = "";
    result += " {";
    for (int i = 0; i < parameters.size(); i++) {
      result += separator;
      result += parameters[i];
      separator = ", ";
    }
    result += "}";
  }
  if (yparameters.size() > 0) {
    string separator = "";
    result += " {";
    for (int i = 0; i < yparameters.size(); i++) {
      result += separator;
      result += yparameters[i];
      separator = ", ";
    }
    result += "}";
  }
  
  
  
  
  return result;
}
//////////////////////////////////////////////////////////////////////////////
//
// Function:
//   Create a spectrum displayer title from the information in the
//   spectrum definition.   As needed items will be dropped from the
//   definition to ensure that this will all fit in the limited
//   number of characters avaialable to a spectrum title.
//
string
CHistogrammer::createTitle(CSpectrum* pSpectrum, UInt_t maxLength)
{
  CSpectrum::SpectrumDefinition def = pSpectrum->GetDefinition();
  string name = def .sName;
  ostringstream typestream;
  typestream << def.eType;
  string type = typestream.str();

  // Create the axis vector:

  vector<string> axes;
  for (int i = 0; i < pSpectrum->Dimensionality(); i++) {
    ostringstream axisstream;
    axisstream << pSpectrum->GetLow(i) << ", " << pSpectrum->GetHigh(i)
	       << " : " << pSpectrum->Dimension(i);
    axes.push_back(axisstream.str());
  }
  // gate name:
 
  const CGateContainer& gate(*(pSpectrum->getGate()));
  string gateName;
  if (&gate != pDefaultGate) {
    gateName = gate.getName();
  } else {
    gateName = "";
  }
  //  Get the parameter names

  vector<UInt_t> ids = def.vParameters;
  vector<UInt_t> yids= def.vyParameters;
  vector<string> parameters;;
  vector<string> yparameters;

  

  for (int i =0; i < ids.size(); i++) {
    CParameter* pParam = FindParameter(ids[i]);
    if (pParam) {
      parameters.push_back(pParam->getName());
    } else {
      parameters.push_back(string("--deleted--"));
    }
  }
  for (int i = 0; i < yids.size(); i++) {
    CParameter* pParam = FindParameter(yids[i]);
    if (pParam) {
      yparameters.push_back(pParam->getName());
    }
    else {
      yparameters.push_back(string("--deleted--"));
    }
  }

 
  // Ok now the following variables are set up for the first try:
  //  name       - Name of the spectrum
  //  type       - String type of the spectrum
  //  axes       - Vector of axis definitions.
  //  gateName       - name of gateName on spectrum.
  //  parameters - vector of parameter names.

  string trialTitle = createTrialTitle(type, axes, parameters, yparameters, gateName);
  if (trialTitle.size() < maxLength) return trialTitle;

  // Didn't fit.one by one drop the parameters..replacing the most recently
  // dropped parameter by "..."

  while (yparameters.size()) {
    yparameters[yparameters.size()-1] = "...";
    trialTitle = createTrialTitle(type, axes, parameters, yparameters, gateName);
    if (trialTitle.size() < maxLength) return trialTitle;
    vector<string>::iterator i = yparameters.end();
    i--;
    yparameters.erase(i);
  }
  while (parameters.size()) {
    parameters[parameters.size()-1] = "..."; // Probably smaller than it was.
    trialTitle = createTrialTitle(type, axes, parameters, yparameters, gateName);
    if (trialTitle.size() < maxLength) return trialTitle;
    vector<string>::iterator i = parameters.end();
    i--;
    parameters.erase(i);	// Kill off last parameter.
  }

  // Still didn't fit... and there are no more parameters left to drop.
  // now we drop the axis definition...
  
  axes.clear();
  trialTitle = createTrialTitle(type , axes, parameters, yparameters, gateName);
  if (trialTitle.size() < maxLength) return trialTitle;

  // Now compute if we can delete the tail of the spectrum name
  // to fit... For this try we drop at most 1/2 of the name.

  if ((trialTitle.size() - (name.size()/2 + 3)) < maxLength) {
    while(trialTitle.size() > maxLength) {
      name = name.assign(name.c_str(), name.size()-4) + string("...");
      trialTitle = createTrialTitle(type, axes, parameters, yparameters, gateName);
    }
    return trialTitle;
  }

  
  // nope...drop the gateName and delete the tail of the spectrum name so it fits.
  //
  
  name.assign(name.c_str(), maxLength - 3) + string("...");
  return name;


  
}

//  Invoke the gate observers when a gate has changed.
// Parameters:
//   name   - name of the gate that's changing.
//   gate   - New gate container.
//
void
CHistogrammer::invokeGateChangedObservers(string name, CGateContainer& gate)
{
  GateObserverList::iterator p = m_gateObservers.begin();
  while (p != m_gateObservers.end()) {
    CGateObserver*  pObserver = (*p);
    pObserver->onChange(name, gate);
    p++;
  }
}
//
// Create and hook in the spectrum and gate observers that ensure we keep our optimized
// gate and spectrum lists up to date.
//
void
CHistogrammer::createListObservers()
{
  m_pGateList = new CFlattenedGateList( this);
  addGateObserver(m_pGateList);

  m_pSpectrumLists = new CSpectrumByParameter;
  addSpectrumDictionaryObserver(m_pSpectrumLists);
}
/**
 * flip2dGatePoints
 *   Determine if the gate point coordinates must be flipped.  This happens
 *   for e.g. a gate on p1, p2 displayed on a spectrum with axes p2, p1
 *
 *  There's an implicit assumption that the gate is displayable on this spectrum
 *  because all we do is see if the X parameter is a match for a spectrum x parameter
 *  and, if not, flip.
 *
 * @param pSpectrum - pointer to the target spectrum.
 * @param gXparam   - Id of the x parameter of the spectrum.
 *
 * @return bool - true if it's necessary to flip axes.
 *
 */
bool
CHistogrammer::flip2dGatePoints(CSpectrum* pSpectrum, UInt_t gXparam)
{
  std::vector<UInt_t> params;
  pSpectrum->GetParameterIds(params);
  if (pSpectrum->getSpectrumType() == ke2Dm) {
    for (int i = 0; i < params.size(); i += 2) {
      if (gXparam == params[i]) return false;
    }
    return true;
  } else {
    return gXparam != params[0];
  }
}
