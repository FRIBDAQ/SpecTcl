//  CSpectrum.cpp
//  Abstract base class for all types of
// spectra.  Spectra include the various
// types of 1-d and 2-d spectra including
// the options for some compressed
// 
//  Spectra are template classes with the
//  actual data type stored in the spectrum
//  given as the class parameterization.  
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#include "Spectrum.h"                               
#include "Event.h"
#include "GateContainer.h"
#include "TrueGate.h"
#include <string.h>

static const char* Copyright = 
"CSpectrum.cpp: Copyright 1999 NSCL, All rights reserved\n";

static string T("-TRUE-");

static CTrueGate       AlwaysMade;
static CGateContainer  TrueContainer(T, 0, AlwaysMade);
CGateContainer*        pDefaultGate = &TrueContainer;

// Functions for class CSpectrum

///////////////////////////////////////////////////////////////////////////
// Function:
//     void operator() (const CEvent& rEvent);
// Operation Type:
//     Evaluator.
//
void
CSpectrum::operator()(const CEvent& rEvent)
{
  if(CheckGate(rEvent)) {
    Increment(rEvent);		// Only increment if gate set.
  }
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CGateContainer* ApplyGate(CGateContainer* pNewGate);
// OPeration Type:
//    Mutator
CGateContainer*
CSpectrum::ApplyGate(CGateContainer* pNewGate)
{
  // Applies  a new gate to the spectrum, returning the prior gate.

  CGateContainer* pOldGate(m_pGate);
  m_pGate = pNewGate;
  return pOldGate;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CGateContainer* Ungate();
// Operation Type:
//    Mutator.
CGateContainer*
CSpectrum::Ungate()
{
  return ApplyGate(&TrueContainer);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void ReplaceStorage ( Address_t pNewLoc, 
//                          Bool_t fTransferOwnership=kfTRUE )
//  Operation Type:
//     mutator
//
void 
CSpectrum::ReplaceStorage(Address_t pNewLoc, Bool_t fTransferOwnership) 
{
// Instructs the spectrum to move the storage associated
// with its channels into the storage pointed to by the
// parameter.  It is up to the caller to ensure that there
// is sufficient storage in the destination.
// Default action is to:
//     memcpy(pNewLoc, m_pStorage, StorageNeeded());
// If the current storage was owned by the spectrum, it is deleted.
// The m_pStorageg and m_fOwnStorage are updated from the parameters.
//
// Formal Parameters:
//       Address_t pNewLoc:
//          Pointer to the new spectrum storage.
//
//       Bool_t fTransferOwnership = kfTRUE:
//           Indicates if the storage should be owned
//           by the spectrum after transfer. 
//

  if(m_pStorage)
    Copy(pNewLoc);
  else {
    memset(pNewLoc, 0, StorageNeeded());
  }
  if(m_fOwnStorage) ReleaseStorage();
  m_pStorage    = pNewLoc;
  m_fOwnStorage = fTransferOwnership;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Clear (  )
//  Operation Type:
//     mutator
//
void 
CSpectrum::Clear() 
{
// Clears the channesl assocaited with the
// spectrum.  By default, this is done via:
//   memset(m_pStorage, 0, StorageRequried());
//
// 

  memset(m_pStorage, 0, StorageNeeded());

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Size_t StorageNeeded (  )
//  Operation Type:
//     Selector
//
Size_t 
CSpectrum::StorageNeeded() const 
{
// Returns the number of storage units required
// to store the data in the histogram.
//
//   E.g. new UChar_t[StorageNeeded()]]
//   could allocate space for the spectrum if it was
//   stored uncompressed.,
//
// Default implementation assumes that the storage is 
// a simple n dimensional array of items.  The only nasty
// bit is figuring out the size of each item, done via a long
// case statement on storage type.
//  

  UInt_t nDims         = Dimensionality(); // total # of dimensions.
  Size_t nStorageUnits = 1;	           // Accumulate storage units here.

  for(UInt_t i = 0; i < nDims; i++) {
    nStorageUnits *= Dimension(i);
  }
  // nStorageUnits must be scaled by the no. of bytes per unit:

  Size_t nBytesPerUnit;
  switch(StorageType()) {
  case keByte:
    nBytesPerUnit = sizeof(Char_t);
    break;
  case keWord:
    nBytesPerUnit = sizeof(Short_t);
    break;
  case keLong:
    nBytesPerUnit = sizeof(Int_t);
    break;
  case keFloat:
    nBytesPerUnit = sizeof(Float_t);
    break;
  case keDouble:
    nBytesPerUnit = sizeof(DFloat_t);
    break;
  }
  return nStorageUnits * nBytesPerUnit;
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    void Copy(void *pStorage) const
// Operation:
//    Data transfer
//

void
CSpectrum::Copy(void* pStorage) const
{
  // Copy the spectrum to external storage:
  //
  memcpy(pStorage, m_pStorage, StorageNeeded());
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//    Bool_t CheckGate(const CEvent& rEvent)
// Operation Type:
//    Gate check.
//
Bool_t
CSpectrum::CheckGate(const CEvent& rEvent)
{
  return (*m_pGate)((CEvent&)rEvent);
}

void
CSpectrum::GammaGateIncrement(const CEvent& rEvent)
{
  operator()(rEvent);
}
