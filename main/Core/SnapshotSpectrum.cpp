/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/
static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";
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

/*
   Change log:
   $Log$
   Revision 5.2  2005/06/03 15:19:23  ron-fox
   Part of breaking off /merging branch to start 3.1 development

   Revision 5.1.2.1  2004/12/21 17:51:25  ron-fox
   Port to gcc 3.x compilers.

   Revision 5.1  2004/11/29 16:56:07  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 4.3  2003/04/19 00:11:13  ron-fox
   Fix a nasty issue with GetDefinition() that was causing death due to a number of problems with the static output struct.  For later: change the struct to a class so that it can be returned by value rather than by reference.. then it wouldn't have to be static.

   Revision 4.2  2003/04/01 19:53:44  ron-fox
   Support for Real valued parameters and spectra with arbitrary binnings.

*/
#include <config.h>
#include "SnapshotSpectrum.h"    				
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

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
/*!
   Get the low limit of an axis.  This just delegates to the
   contained spectrum.
   \param <TT> nDimension (UInt_t [in]) </TT>
      Selects the dimension for which we want the high limit.
      You can call Dimensionality to determine the number of
      valid dimensions.  

   \throw CRangeError
      An invalid nDimension parameter will
      cause the spectrum to throw a CRangeError exception.
   \retval Float_t
      The least parameter value that will cause the first channel
      (channel 0) of the selected axis to be incremented.
*/
Float_t
CSnapshotSpectrum::GetLow(UInt_t nDimension) const
{
  return m_rActualSpectrum.GetLow(nDimension);
}

/*!
   Get the high limit of an axis.  This just delegates to the
   contained spectrum.
   \param <TT> nDimension (UInt_t [in]) </TT>
      Selects the dimension for which we want the high limit.
      You can call Dimensionality to determine the number of
      valid dimensions.  

   \throw CRangeError
      An invalid nDimension parameter will
      cause the spectrum to throw a CRangeError exception.
   \retval Float_t
      The largest parameter value that will cause the last channel
       of the selected axis to be incremented.
*/
Float_t
CSnapshotSpectrum::GetHigh(UInt_t nDimension) const
{
  return m_rActualSpectrum.GetHigh(nDimension);
}
/*!
   Get the spectrum units:
*/
string
CSnapshotSpectrum::GetUnits(UInt_t nDimension) const
{
  return m_rActualSpectrum.GetUnits(nDimension);
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


CSpectrum::SpectrumDefinition&
CSnapshotSpectrum::GetDefinition() {
  return m_rActualSpectrum.GetDefinition();
}
