// Class: CSnapshotSpectrum
// Snapshot spectrum.  This contains another spectrum which is
// never incremented.  All other operations are delegated to 
// the underlying spectrum, however Increment is a no-op.
//
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "SnapshotSpectrum.h"    				

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved SnapshotSpectrum.cpp \n";

// Functions for class CSnapshotSpectrum

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     UsesParameter(const CParameter& rParameter)
//  Operation Type: 
//     Selector.
Bool_t CSnapshotSpectrum::UsesParameter(const CParameter& rParameter) const
{
  // Returns TRUE if the parameter passed in is
  // required in order to increment the spectrum.
  // This is usually used when deleting a parameter
  // from the parameter dictionary to determine if
  // the spectrum should also be deleted.
  // It is up to the actual implementation of a gated histgoram
  // to decide whether or not a parameter required for a gate
  // also constitutes neededness.
  //
  // Formal Parameters:
  //     const CParameter& rParam:
  //         Refers to the parameter for which need
  //         is inquired.
  // Returns:
  //     kfTRUE - if the parameter is required by the
  //                     spectrum.
  //     kfFALSE- If the parameter is not requried.
  //
  return m_rActualSpectrum.UsesParameter(rParameter);
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ReplaceStorage(Address_t pNewLoc, Bool_t fTransferOwnership)
//  Operation Type: 
//     mutator
void CSnapshotSpectrum::ReplaceStorage(Address_t pNewLoc, 
				       Bool_t fTransferOwnership)  
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
  m_rActualSpectrum.ReplaceStorage(pNewLoc, fTransferOwnership);
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Clear()
//  Operation Type: 
//     mutator
void CSnapshotSpectrum::Clear()  
{
  // Clears the channels assocaited with the
  // spectrum.  By default, this is done via:
  //   memset(m_pStorage, 0, StorageRequried());
  //
  // NOTE: For snapshot spectra, this leaves an irretrievably empty spectrum
  //       unless the underlying spectrum is gotten and modified.

  m_rActualSpectrum.Clear();
  
}

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Dimension(UInt_t nDimension)
//  Operation Type: 
//     Selector
Size_t CSnapshotSpectrum::Dimension(UInt_t nDimension) const  
{
  // Returns the number of channels in a given dimension.
  // Dimensions are numbered from zero.
  //
  //     Uint_t    nDimension
  //         Selects the dimension.
  
  return m_rActualSpectrum.Dimension(nDimension);
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Dimensionality()
//  Operation Type: 
//     Selector.
UInt_t CSnapshotSpectrum::Dimensionality() const  
{
  // Returns the number of dimensions in the spectrum.
  //
  return m_rActualSpectrum.Dimensionality();
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator[](const UInt_t* pIndices)
//  Operation Type: 
//     Indexing operator.
ULong_t CSnapshotSpectrum::operator[](const UInt_t* pIndices)  const
{
  // The value of a  single channel
  // in the histogram.
  // 
  // Formal Parameters:
  //      UInt_t* pIndicesr
  //           A list of indices.  It is up to the caller
  //           to ensure that sufficient indices are supplied.
  // 
  return  m_rActualSpectrum[pIndices];
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     StorageNeeded()
//  Operation Type: 
//     Selector
Size_t CSnapshotSpectrum::StorageNeeded() const  
{
  // Returns the number of storage units required
  // to store the data in the histogram.
  //
  //   E.g. new UChar_t[StorageNeeded()]]
  //   could allocate space for the spectrum if it was
  //   stored uncompressed.,
  //
  return m_rActualSpectrum.StorageNeeded();
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Increment(const CEvent& rEvent)
//  Operation Type: 
//     incrementer
void CSnapshotSpectrum::Increment(const CEvent& rEvent)  
{
  // Increments the spectrum given an event.
  //
  // Formal Parameters:
  //     CEvent& rEvent:
  //          Refers to the event to use to increment.
  //
  // This is a no-op... snapshots are never incremented.xs
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   SpectrumType_t getSpectrumType()
// Operation Type:
//   override/delegator.
//
SpectrumType_t
CSnapshotSpectrum::getSpectrumType()
{
  // Returns the underlying spectrum type:

  return m_rActualSpectrum.getSpectrumType();
}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   Bool_t UsesParameter(UInt_t nId) const
// Operation Type:
//   Utility
//
Bool_t
CSnapshotSpectrum::UsesParameter(UInt_t nId) const
{
  return m_rActualSpectrum.UsesParameter(nId);
}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   void set(const UInt_t* pIndices, ULong_t nValue)
// Operation Type:
//   Utility:
//
void
CSnapshotSpectrum::set(const UInt_t* pIndices, ULong_t nValue)
{
  m_rActualSpectrum.set(pIndices, nValue);
}

//////////////////////////////////////////////////////////////////////////

void
CSnapshotSpectrum::GetParameterIds(vector<UInt_t>& rvIds)
{
  m_rActualSpectrum.GetParameterIds(rvIds);
}
//////////////////////////////////////////////////////////////////////////

void 
CSnapshotSpectrum::GetResolutions(vector<UInt_t>&  rvResolutions)
{
  m_rActualSpectrum.GetResolutions(rvResolutions);
}
//////////////////////////////////////////////////////////////////////////

Int_t
CSnapshotSpectrum::getScale(UInt_t index)
{
  return m_rActualSpectrum.getScale(index);
}
