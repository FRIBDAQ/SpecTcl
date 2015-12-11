/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2015.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Jeromy Tompkins
         NSCL
         Michigan State University
         East Lansing, MI 48824-1321
*/

#include <DisplayInterface.h>
#include <DisplayGate.h>
#include <Display.h>
#include <Histogrammer.h>
#include <Dictionary.h>
#include <DictionaryException.h>
#include <CFitDictionary.h>
#include <Histogrammer.h>
#include <CSpectrumFit.h>
#include <XamineGates.h>
#include <PointlistGate.h>
#include <Cut.h>
#include <Gamma2DW.h>
#include <GateMediator.h>

#include <Xamine1D.h>
#include <Xamine2D.h>

#include <string>
#include <assert.h>
#include <sstream>

using namespace std;

// Static member data:

int CDisplayInterface::m_nextFitlineId(1); // Next fitline id assigned.


CDisplayInterface::CDisplayInterface() {}

CDisplayInterface::CDisplayInterface(const CDisplayInterface&) {}

CDisplayInterface::~CDisplayInterface() {}



CDisplay *CDisplayInterface::getDisplay() const
{
    return m_pDisplay;
}

void CDisplayInterface::setDisplay(CDisplay *pDisplay)
{
    m_pDisplay = pDisplay;
}


/*!
    Binds a spectrum to the display.
    The first free spectrum will be allocated.
   If the spectrum is already bound, then we short circuit and
   just return the current binding number:
   This is not an error, just a user mistake we can handle.

  
    \param sName : const std::string
             Name of the spectrum to bind.

    \return   UInt_t  - Actual spectrum number chosen.

   
   \throw   CDictionaryException - if spectrum of given name does not exist.
   \throw  CErrnoException      - may be thrown by routines we call.

  
  */
UInt_t CDisplayInterface::BindToDisplay(const std::string& rsName) {

  for(int i = 0; i < m_DisplayBindings.size(); i++) {
    if(rsName == m_DisplayBindings[i])
      return i;
  }
  // The spectrum must exist or a dictionary exception is thrown.
  //

  SpectrumDictionary dictionary = m_pSorter->getSpectrumDictionary();
  SpectrumDictionaryIterator iSpectrum = dictionary.Lookup(rsName);
  if(iSpectrum == dictionary.end()) {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
			       "CDisplayInterface::BindToDisplay Locating spectrum",
			       rsName);
  }

  // From the spectrum we must construct an Xamine spectrum which desribes
  // what we're trying to create.  There are two variables to worry about
  //  Dimensionality (determines the kind of Xamine spectrum to be created)
  //  and StorageType (determines how to construct it among other things).
  //  asserts are used to enforce restrictions on the types of spectra
  //  supported by Xamine.
  UInt_t nSpectrum;       
  CSpectrum*       pSpectrum = (*iSpectrum).second;
  CXamineSpectrum* pXSpectrum(0);
  try {
    switch(pSpectrum->Dimensionality()) {
    case 1:			// 1-d spectrum.
      {
	Bool_t           fWord = pSpectrum->StorageType() == keWord;
    pXSpectrum   = new CXamine1D(m_pDisplay->getXamineMemory(),
				     rsName,
				     pSpectrum->Dimension(0),
				     pSpectrum->GetLow(0),
				     pSpectrum->GetHigh(0),
				     pSpectrum->GetUnits(0),
				     fWord);

	
	break;
      }
    case 2:			// 2-d spectrum.
      {
	// 2d spectra can now have any data type so:

	int dataType;
	switch (pSpectrum->StorageType()) {
	case keWord:			// 2dW
	  dataType = 0;
	  break;
	case keByte:		// 2db
	  dataType = 1;
	  break;
	case keLong:			// (was 2).  2dL
	  dataType = 2;
	  break;
	default:
	  throw string("Invalid 2d spectrum type");
	}

    pXSpectrum = new CXamine2D(m_pDisplay->getXamineMemory(),
				   rsName,
				   pSpectrum->Dimension(0),
				   pSpectrum->Dimension(1),
				   pSpectrum->GetLow(0),
				   pSpectrum->GetLow(1),
				   pSpectrum->GetHigh(0),
				   pSpectrum->GetHigh(1),
				   pSpectrum->GetUnits(0),
				   pSpectrum->GetUnits(1),
				   dataType);

	break;
      default:			// Unrecognized dimensionality.
	assert(kfFALSE);
      }
    }
    // pXSpectrum points to a spectrum which can be 'defined' in Xamine:
    // pSpectrum  points to a spectrum dictionary entry.
    // What's left to do is:
    //   Define the spectrum to Xamine (allocating slot and storage).
    //   Replace the spectrum's storage with Xamine's.
    //   Enter the slot/name correspondence in the m_DisplayBindings table.
    //
			      
    Address_t pStorage           = m_pDisplay->DefineSpectrum(*pXSpectrum);
    nSpectrum                    = pXSpectrum->getSlot();

    m_pDisplay->setInfo(createTitle(pSpectrum, m_pDisplay->getTitleSize()),
                        nSpectrum);
    pSpectrum->ReplaceStorage(pStorage, kfFALSE);
    while(m_DisplayBindings.size() <= nSpectrum) {
      m_DisplayBindings.push_back("");
      m_boundSpectra.push_back(0);
    }
    m_DisplayBindings[nSpectrum] = pSpectrum->getName();
    m_boundSpectra[nSpectrum]    = pSpectrum;
    delete pXSpectrum;		// Destroy the XamineSpectrum.
  }
  catch (...) {		// In case of throw after CXamine2D created.
    delete pXSpectrum;
    throw;
  }
  // We must locate all of the gates which are relevant to this spectrum
  // and enter them as well:
  //

  vector<CGateContainer> DisplayGates = GatesToDisplay(rsName);

  UInt_t Size = DisplayGates.size();
  for(UInt_t i = 0; i < DisplayGates.size(); i++) {
    CDisplayGate* pXgate = GateToXamineGate(nSpectrum, DisplayGates[i]);
    if(pXgate) m_pDisplay->EnterGate(*pXgate);
    delete pXgate;
  }
  // same for the fitlines:
  //

  CFitDictionary& dict(CFitDictionary::getInstance());
  CFitDictionary::iterator pf = dict.begin();

  while (pf != dict.end()) {
    CSpectrumFit* pFit = pf->second;
    if (pFit->getName() == pSpectrum->getName()) {
      addFit(*pFit);		// not very efficient, but doesn't need to be
    }
    pf++;
  }

  return nSpectrum;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void UnBindFromDisplay ( UInt_t nSpec )
//  Operation Type:
//     mutator
//
void CDisplayInterface::UnBindFromDisplay(UInt_t nSpec) {
  // Unbinds the spectrum which is 
  // attached to the specified Displayer spectrum number.
  // 
  // Formal Parameters:
  //    UInt_t nSpec:
  //       Display spectrum id to unbind.

  CXamineSpectrum  Spec(m_pDisplay->getXamineMemory(), nSpec);
  if(Spec.getSpectrumType() != undefined) { // No-op if spectrum not defined

    SpectrumDictionary dictionary = m_pSorter->getSpectrumDictionary();
    SpectrumDictionaryIterator iSpectrum = dictionary.Lookup(m_DisplayBindings[nSpec]);
    assert(iSpectrum != dictionary.end());

    CSpectrum*       pSpectrum = (*iSpectrum).second;
    //
    //  What we need to do is:
    //    0. Remove the gates which are being displayed.
    //    1. Provide local storage for the spectrum data.
    //    2. Remove the spectrum from the bindings table.
    //    3. Tell Xamine to free the slot and spectrum.
    //
    
    // Deal with the gates:
    
    CXamineGates* pGates = m_pDisplay->GetGates(nSpec);
    CDisplayGateVectorIterator pGateIterator = pGates->begin();
    while(pGateIterator != pGates->end()) {
      UInt_t   nGateId   = pGateIterator->getId();
      GateType_t eGateType = pGateIterator->getGateType();
      m_pDisplay->RemoveGate(nSpec, nGateId, eGateType);
      pGateIterator++;
    }

    delete pGates;

    // Deal with the spectrum:

    pSpectrum->ReplaceStorage(new char[pSpectrum->StorageNeeded()],
			      kfTRUE);
    m_DisplayBindings[nSpec] = "";
    m_boundSpectra[nSpec]    = 0;
    m_pDisplay->FreeSpectrum(nSpec);

  }

}

/*!
   addFit : adds a fit to the Xamine bindings.  We keep track of
   these fits in m_fitlineBindings.  This is a vector of lists.
   The index of each vector element is the Xamine 'display slot' fitlines
   are bound to. Each element is a list of pairs.  Each pair is the fitline
   id and fitline name.
   \param fit : CSpectrumFit&
     Reference to the fit to add.
*/
void
CDisplayInterface::addFit(CSpectrumFit& fit)
{
  // get the fit name and spectrum name... both of which we'll need to
  //   ensure we can add/bind the fit.

  string fitName      = fit.fitName();
  string spectrumName = fit.getName();
  Int_t  xSpectrumId  = findDisplayBinding(spectrumName);
  if (xSpectrumId < 0) {
    // Display is not bound to Xamine.

    return;
  }
  // The display is bound... ensure that our fitlines binding array is large
  // enough.

  while (m_FitlineBindings.size() <=  xSpectrumId) {
    FitlineList empty;
    m_FitlineBindings.push_back(empty);
  }

  // Now we must:
  //  1. Allocate a fitline id.
  //  2. Enter the fit line in Xamine.
  //  3. Add the fitline name/id to our m_FitlineBindings 

  int fitId = m_nextFitlineId++;
  Xamine_EnterFitline(xSpectrumId+1, fitId,
		      const_cast<char*>(fitName.c_str()),
		      fit.low(), fit.high(),
		      const_cast<char*>(fit.makeTclFitScript().c_str()));
  pair <int, string> fitInfo(fitId, fitName);
  m_FitlineBindings[xSpectrumId].push_back(fitInfo);

  
}
/*!
  Remove a fit.  It is  a no-op to delete a fit that does not exist or is
  on an unbound spectrum. The rough cut of what we will do is
  - Locate the spectrum id of the binding.
  - Locate any fit that matches the name of the fit we are given
    in the fit bindings list assocated with that spectrum.
  - Ask Xamine to delete that fit (fits are like gates).
  - Remove this fit from our bindings list.
  \param fit : CSpectrumFit&
     referenced to the fit to remove.
*/
void
CDisplayInterface::deleteFit(CSpectrumFit& fit)
{
  string spectrumName =  fit.getName();
  string fitName      = fit.fitName();
  int    xSpectrumId  = findDisplayBinding(spectrumName);
  if (xSpectrumId >= 0 && ( xSpectrumId <  m_FitlineBindings.size())) {

    // xSpectrumId < 0 means spectrum not bound.
    // xSpectrumId >= size of the bindings vector means no fitlines on spectrum.

    FitlineList::iterator i = m_FitlineBindings[xSpectrumId].begin();
    FitlineList::iterator e = m_FitlineBindings[xSpectrumId].end();
    while (i != e) {
      if (fitName == i->second) {
	// found it.. delete this one and return... don't delete all
	// occurences as 
	// a. there's only supposed to be one occurence.
	// b. Depending on the underlying representation of a FitlineList,
	//    deletion may invalidate i.

	Xamine_RemoveGate(xSpectrumId+1, i->first,
			  fitline);
	m_FitlineBindings[xSpectrumId].erase(i);
	return;
      }
      i++;
    }
    // Falling through here means no matching fit lines...which is a no-op.
    
  }
}

/**
 * updateStatistics
 *    Update the Xamine statistics for each bound spectrum.
 */
void
CDisplayInterface::updateStatistics()
{
    for (int i =0; i < m_boundSpectra.size(); i++) {
        CSpectrum* pSpec = m_boundSpectra[i];
        if (pSpec) {
            std::vector<unsigned> stats = pSpec->getUnderflows();
            m_pDisplay->setUnderflows(i, stats[0], (stats.size() == 2 ? stats[1] : 0));
            stats = pSpec->getOverflows();
            m_pDisplay->setOverflows(i, stats[0], (stats.size() == 2 ? stats[1] : 0));
        }
    }
}

////////////////////////////////////////////////////////////////////////
//
// :Function:
//    CDisplayGate* GateToXamineGate(UInt_t nBindingId, CGateContainer& rGate)
//  Operation Type:
//    Protected Utility
//
/*!
  Takes a gate container and turns it into a gate suitable for
  entry in the Xamine display program.
   
  Formal Parameters:
  \param <TT> nBindingId: (UInt_t [in]): </TT>
  The Xamine binding identifier of the spectrum (xamine spectrum
  slot number.
  \param <TT> rGate (CGateContainer& [in]): </TT>
  The container which holds the gate to convert.  Note that
  gate containers can be treated as if they were pointers to
  gates.
   
  \retval    CDisplayGate*  kpNULL -- if the gate is not convertable,
  e.g. it is not a gate directly suported
  by Xamine.
  other  -- Pointer to the gate which was created.
  \note
  The gate is dynamically allocated and therefore must be deleted by
  the client.
  
*/
CDisplayGate* CDisplayInterface::GateToXamineGate(UInt_t nBindingId,
					      CGateContainer& rGate)
{
  CDisplayGate* pXGate;
  CSpectrum*    pSpectrum = m_pSorter->FindSpectrum(m_DisplayBindings[nBindingId]);
  assert(pSpectrum != (CSpectrum*)kpNULL);

  // Summary spectra don't have gates displayed.

  if(pSpectrum->getSpectrumType() == keSummary) { // Summary spectrum.
    return (CDisplayGate*)(kpNULL);
  }

  if((rGate->Type() == std::string("s")) ||
     (rGate->Type() == std::string("gs"))) {	// Slice gate
    CDisplayCut* pCut = new CDisplayCut(nBindingId, 
					rGate.getNumber(),
					rGate.getName());
    // There are two points, , (low,0), (high,0)

    CCut& rCut = (CCut&)(*rGate);

    switch(pSpectrum->getSpectrumType()) {
    case ke1D:
    case keG1D: 
      {
	// Produce the nearest channel to the gate points.
	// then add them to the display gate.
	//
	int x1 = (int)(pSpectrum->ParameterToAxis(0, rCut.getLow()));
	int x2 = (int)(pSpectrum->ParameterToAxis(0, rCut.getHigh()));
	pCut->AddPoint(x1,  
		       0);
	// The weirdness below is all about dealing with a special boundary
	// case when we try to get the right side of the cut to land
	// on the right side of the channel on which it's set.
	// ..all this in the presence of gates accepted on fractional parameters.
	//(consider a fine spectrum (e.g. 400-401 with 100 bins and a coarse
	// spectrum, of the same parameter (e.g. 0-1023 1024 bins)..with 
	// the gate set on 400.5, 400.51 and you'll see the thing I'm trying
	// to deal with here.
	// 
	pCut->AddPoint(x1 == x2 ? x2 : x2 - 1, 
		       0);
	return pCut;
	break;
      }

    }
  }
  else if ((rGate->Type() == std::string("b")) ||
	   (rGate->Type() == std::string("gb"))) { // Band gate.
    pXGate = new CDisplayBand(nBindingId,
			      rGate.getNumber(),
			      rGate.getName());
  }
  else if ((rGate->Type() == std::string("c")) ||
	   (rGate->Type() == std::string("gc"))) { // Contour gate
    pXGate = new CDisplayContour(nBindingId,
				 rGate.getNumber(),
				 rGate.getName());
  }
  else {			// Other.
    return (CDisplayGate*)kpNULL;
  }
  // Control falls through here if 2-d and we just need
  // to insert the points.  We know this is a point list gate:

  assert((rGate->Type() == "b") || (rGate->Type() == "c") ||
	 (rGate->Type() == "gb") || (rGate->Type() == "gc"));

  // If the spectrum is not 2-d the gate can't be displayed:
  //

  if((pSpectrum->getSpectrumType() == ke2D)   ||
     (pSpectrum->getSpectrumType() == keG2D)  ||
     (pSpectrum->getSpectrumType() == ke2Dm)  ||
     (pSpectrum->getSpectrumType() == keG2DD)) {
    

    CPointListGate& rSpecTclGate = (CPointListGate&)rGate.operator*();
    vector<FPoint> pts = rSpecTclGate.getPoints();
    //    vector<UInt_t> Params;
    //    pSpectrum->GetParameterIds(Params);

    // If necessary flip the x/y coordinates of the gate.
    // note that gamma gates never need flipping.
    //
    
    //    if((rSpecTclGate.getxId() != Params[0]) &&
    //   ((rSpecTclGate.Type())[0] != 'g')) {

    if ((rSpecTclGate.Type()[0] != 'g') && 
	flip2dGatePoints(pSpectrum, rSpecTclGate.getxId())) {
      for(UInt_t i = 0; i < pts.size(); i++) {	// Flip pts to match spectrum.
	Float_t x = pts[i].X();
	Float_t y = pts[i].Y();
	pts[i] = FPoint(y,x);
      }
    }
    // The index of the X axis transform is easy.. it's 0, but the
    // y axis transform index depends on spectrum type sincd gammas
    // have all x transforms first then y and so on:
    //
    int nYIndex;
    if((pSpectrum->getSpectrumType() == ke2D)   ||
       (pSpectrum->getSpectrumType() == keG2DD) ||
       (pSpectrum->getSpectrumType() == ke2Dm)) {
      nYIndex = 1;
    }
    else {
      CGamma2DW* pGSpectrum = (CGamma2DW*)pSpectrum;
      nYIndex               = pGSpectrum->getnParams();
    }
    
    for(UInt_t i = 0; i < pts.size(); i++) {
      
      CPoint pt((int)pSpectrum->ParameterToAxis(0, pts[i].X()),
		(int)pSpectrum->ParameterToAxis(nYIndex, pts[i].Y()));
      pXGate->AddPoint(pt);
      
    }
  } else {
    return (CDisplayGate*)kpNULL;
  }

  return pXGate;
}

////////////////////////////////////////////////////////////////////////////
//
// Function:
//   void AddGateToBoundSpectra(CGateContainer& rGate)
// Operation Type:
//   protected utility
//
void CDisplayInterface::AddGateToBoundSpectra(CGateContainer& rGate) {
  // Takes a (presumably) newly created gate, and enters it into the
  // appropriate set of spectra bound to Xamine.
  //
  // Formal Parameters:
  //    CGateContainer& rGate:
  //       Container which holds the gate.  Note that gate containers can
  //       be treated as if they were pointers to gates.
  //

  // The mediator tells us whether the spectrum can display the gate
  for(UInt_t nId = 0; nId < m_DisplayBindings.size(); nId++) {
    if(m_DisplayBindings[nId] != "") { // Spectrum bound.
      CSpectrum* pSpec = m_pSorter->FindSpectrum(m_DisplayBindings[nId]);
      assert(pSpec != (CSpectrum*)kpNULL); // Bound spectra must exist!!.
      CGateMediator DisplayableGate(rGate, pSpec);
      if(DisplayableGate()) {
	CDisplayGate* pDisplayed = GateToXamineGate(nId, rGate);
	if(pDisplayed)
      m_pDisplay->EnterGate(*pDisplayed);
	delete pDisplayed;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////
// 
// Function:
//    void RemoveGateFromBoundSpectra(CGateContainer& rGate)
// Operation Type:
//    Protected Utility
//
void CDisplayInterface::RemoveGateFromBoundSpectra(CGateContainer& rGate) {
  // Removes a gate which is just about to be destroyed from
  // the appropriate set of Xamine bound spectra.
  //
  // Formal Paramters:
  //    CGateContainer& rGate:
  //       Reference to the container which holds the gate about to be
  //       destroyed.  Note that for most purposes, a gate container
  //       can be treated as if it was a pointer to a gate.
  // 
  UInt_t nGateId = rGate.getNumber();
  GateType_t eType;
  if(rGate->Type() == "c" || rGate->Type() == "gc") {
    eType = kgContour2d;
  }
  else if(rGate->Type() == "b" || rGate->Type() == "gb") {
    eType = kgBand2d;
  }
  else if (rGate->Type() == "s" || rGate->Type() == "gs") {
    eType = kgCut1d;
  }
  else {
    return;			// Non -primitive gates won't be displayed.
  }

  // This function is quite simple since gates entered in Xamine on our
  // behalf will have this id and name.   Therefore we just need
  // to remove gates with id == nGateId from all spectra bound.
  //
  // Note that CXamine::RemoveGate throws on error, and therefore
  // we must catch and ignore exceptions at the removal.

  for(UInt_t nId = 0; nId < m_DisplayBindings.size(); nId++) {
    if(m_DisplayBindings[nId] != "") {
      try {
    m_pDisplay->RemoveGate(nId, nGateId, eType);
      }
      catch(...) {		// Ignore exceptions.
      }
    }
  }
}



/////////////////////////////////////////////////////////////////////////////
//
//  Function:
//    std::vector<CGateContainer> GatesToDisplay(const std::string& rSpectrum)
// Operation Type:
//    Protected utility.
//
std::vector<CGateContainer>
CDisplayInterface::GatesToDisplay(const std::string& rSpectrum)
{
  // Returns a vector of gates which can be displayed on the spectrum.
  // Gates are considered displayable on a spectrum iff the gate parameter set
  // is the same as the spectrum parameter set. If the spectrum is a gamma 
  // spectrum, then the gate is only displayed on one spectrum. Note that 
  // displayable gates at present are one of the following types only:
  //   Contour (type = 'c')
  //   Band    (type = 'b')
  //   Cut     (type = 's')
  //   Sum2d   {type = 'm2'}
  //   GammaContour   (type = 'gc')
  //   GammaBand      (type = 'gb')
  //   GammaCut       (type = 'gs')
  // All other gates are not displayable.
  //

  std::vector<CGateContainer> vGates;
  CSpectrum *pSpec = m_pSorter->FindSpectrum(rSpectrum);
  if(!pSpec) {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
			       "No such spectrum CDisplayInterface::GatesToDisplay",
			       rSpectrum);
  }
  //
  // The mediator tells us whether the spectrum can display the gate:
  //
  CGateDictionaryIterator pGate = m_pSorter->GateBegin();
  while(pGate != m_pSorter->GateEnd()) {
    CGateMediator DisplayableGate(((*pGate).second), pSpec);
    if(DisplayableGate()) {
      vGates.push_back((*pGate).second);
    }
    pGate++;
  }
  
  return vGates;
}

//////////////////////////////////////////////////////////////////////
//
// Function:
//      CSpectrum*  DisplayBinding(UInt_t xid)
// Operation type:
//      Selector.
//
CSpectrum* CDisplayInterface::DisplayBinding(UInt_t xid) {
  // Returns  a pointer to a spectrum which is bound on a particular
  // xid.
  // Formal Parameters:
  //    UInt_t xid:
  //       The xid.
  // Returns:
  //     POinter to the spectrum or kpNULL Iff:
  //     xid is out of range.
  //     xid does not map to a spectrum.
  //
  if(xid >= DisplayBindingsSize()) 
    return (CSpectrum*)kpNULL;

  return m_boundSpectra[xid];
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    DisplayBindingsIterator DisplayBindingsBegin()
//  Operation Type:
//     Selector
//
DisplayBindingsIterator CDisplayInterface::DisplayBindingsBegin() {
  // Returns a begining iterator to support iterating through the set of
  // display bindings.
  return m_DisplayBindings.begin();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    DisplayBindingsIterator DisplayBindingsEnd (  )
//  Operation Type:
//     
//
DisplayBindingsIterator CDisplayInterface::DisplayBindingsEnd() {
  // Returns an iterator which can be used to determin
  // if the end of the display bindings set has been iterated through.
  return m_DisplayBindings.end();
}

/*!
    Find the bindings for a spectrum by name.
   \param name  : string
       Name of the spectrum
   \return 
   \retval -1   - Spectrum has no binding.
   \retval >= 0 - The binding index (xamine slot).

*/
Int_t
CDisplayInterface::findDisplayBinding(string name)
{
  for (int i = 0; i < DisplayBindingsSize(); i++) {
    if (name == m_DisplayBindings[i]) {
      return i;
    }
  }
  return -1;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t DisplayBindingsSize (  )
//  Operation Type:
//     
//
UInt_t CDisplayInterface::DisplayBindingsSize() {
  // Returns the number of spectra bound to the display.
  return m_DisplayBindings.size();
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
CDisplayInterface::createTitle(CSpectrum* pSpectrum, UInt_t maxLength)
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
    CParameter* pParam = m_pSorter->FindParameter(ids[i]);
    if (pParam) {
      parameters.push_back(pParam->getName());
    } else {
      parameters.push_back(string("--deleted--"));
    }
  }
  for (int i = 0; i < yids.size(); i++) {
    CParameter* pParam = m_pSorter->FindParameter(yids[i]);
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
CDisplayInterface::createTrialTitle(string type, vector<string>      axes,
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
CDisplayInterface::flip2dGatePoints(CSpectrum* pSpectrum, UInt_t gXparam)
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

int CDisplayInterface::GetEventFd()
{
    m_pDisplay->GetEventFd();
}
