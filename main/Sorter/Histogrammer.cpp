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
//    CHistogrammer(const CDisplayInterface&  rDisplayer)
//  Operation Type:
//    Construtor
//

CHistogrammer::CHistogrammer()
{
  srand(time(NULL)); 
//  m_pFitObserver = new CHistogrammerFitObserver(*m_pDisplayer); // Follow changes to fits.
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
CHistogrammer::CHistogrammer(const CHistogrammer& rRhs)
{
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
//    Int_t b  = findDisplayBinding(pSpectrum->getName());
//    if (b >= 0) {
//      m_pDisplayer->setTitle(pSpectrum->getName(), b);
//      m_pDisplayer->setInfo(createTitle(pSpectrum,
//					 m_pDisplayer->getTitleSize()), b);
//    }
    
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
//  AddGateToBoundSpectra(aGate);
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

  //RemoveGateFromBoundSpectra(*pContainer);
  pContainer->setGate(&rGate);
  invokeGateChangedObservers(rGateName, *pContainer);
//  AddGateToBoundSpectra(*pContainer);
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
//  Int_t b =  findDisplayBinding(rSpectrum);
//  if(b >= 0) {
//    m_pDisplayer->setTitle(pSpectrum->getName(), b);
//    m_pDisplayer->setInfo(createTitle(pSpectrum,
//				      m_pDisplayer->getTitleSize()),
//			  b);
//  }
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
