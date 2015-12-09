#include "DisplayManager.h"

DisplayManager::DisplayManager()
{
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
UInt_t CHistogrammer::BindToDisplay(const std::string& rsName) {

  for(int i = 0; i < m_DisplayBindings.size(); i++) {
    if(rsName == m_DisplayBindings[i])
      return i;
  }
  // The spectrum must exist or a dictionary exception is thrown.
  //

  SpectrumDictionaryIterator iSpectrum = m_SpectrumDictionary.Lookup(rsName);
  if(iSpectrum == m_SpectrumDictionary.end()) {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
                   "CHistogrammer::BindToDisplay Locating spectrum",
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
    pXSpectrum   = new CXamine1D(m_pDisplayer->getXamineMemory(),
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

    pXSpectrum = new CXamine2D(m_pDisplayer->getXamineMemory(),
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

    Address_t pStorage           = m_pDisplayer->DefineSpectrum(*pXSpectrum);
    nSpectrum                    = pXSpectrum->getSlot();

    m_pDisplayer->setInfo(createTitle(pSpectrum,
                      m_pDisplayer->getTitleSize()), nSpectrum);
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
    if(pXgate) m_pDisplayer->EnterGate(*pXgate);
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
void CHistogrammer::UnBindFromDisplay(UInt_t nSpec) {
  // Unbinds the spectrum which is
  // attached to the specified Displayer spectrum number.
  //
  // Formal Parameters:
  //    UInt_t nSpec:
  //       Display spectrum id to unbind.

  CXamineSpectrum  Spec(m_pDisplayer->getXamineMemory(), nSpec);
  if(Spec.getSpectrumType() != undefined) { // No-op if spectrum not defined


    SpectrumDictionaryIterator iSpectrum =
      m_SpectrumDictionary.Lookup(m_DisplayBindings[nSpec]);
    assert(iSpectrum != m_SpectrumDictionary.end());

    CSpectrum*       pSpectrum = (*iSpectrum).second;
    //
    //  What we need to do is:
    //    0. Remove the gates which are being displayed.
    //    1. Provide local storage for the spectrum data.
    //    2. Remove the spectrum from the bindings table.
    //    3. Tell Xamine to free the slot and spectrum.
    //

    // Deal with the gates:

    CXamineGates* pGates = m_pDisplayer->GetGates(nSpec);
    CDisplayGateVectorIterator pGateIterator = pGates->begin();
    while(pGateIterator != pGates->end()) {
      UInt_t   nGateId   = pGateIterator->getId();
      GateType_t eGateType = pGateIterator->getGateType();
      m_pDisplayer->RemoveGate(nSpec, nGateId, eGateType);
      pGateIterator++;
    }

    delete pGates;

    // Deal with the spectrum:

    pSpectrum->ReplaceStorage(new char[pSpectrum->StorageNeeded()],
                  kfTRUE);
    m_DisplayBindings[nSpec] = "";
    m_boundSpectra[nSpec]    = 0;
    m_pDisplayer->FreeSpectrum(nSpec);

  }

}
