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

static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";
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

/*
   Change log:
    $Log$
    Revision 5.3  2006/09/20 10:43:49  ron-fox
    Just some formatting (replacing licenses etc). stuff.

    Revision 5.2  2005/06/03 15:19:23  ron-fox
    Part of breaking off /merging branch to start 3.1 development

    Revision 5.1.2.2  2005/05/02 20:14:41  ron-fox
    Little changes to support gcc 3.4 compiler which is a bit stricter even.

    Revision 5.1.2.1  2004/12/21 17:51:25  ron-fox
    Port to gcc 3.x compilers.

    Revision 5.1  2004/11/29 16:56:08  ron-fox
    Begin port to 3.x compilers calling this 3.0

    Revision 4.5.4.1  2004/10/27 12:38:40  ron-fox
    optimize performance of Spectrum1DL histogram increments.  Total
    performance gain was a factor of 2.8.  The 'unusual' modifications
    are documented via comments that indicate they were suggested by profile
    data.

    Revision 4.5  2004/01/27 19:33:18  ron-fox
    Correct Bug 101 (for now) randomization was too agressive.  For now remove it
    completely.  Also discover and fix a channel allocation error.

    Revision 4.4  2003/10/28 21:35:53  ron-fox
    #include <time.h> for randomizer... apparently
    this comes in from stdlib.h on linux but should
    come from time.h according to all man pages.

    Revision 4.3  2003/04/19 00:11:13  ron-fox
    Fix a nasty issue with GetDefinition() that was causing death due to a number of problems with the static output struct.  For later: change the struct to a class so that it can be returned by value rather than by reference.. then it wouldn't have to be static.

    Revision 4.2  2003/04/01 19:53:46  ron-fox
    Support for Real valued parameters and spectra with arbitrary binnings.

*/

//
// Header Files:
//
#include <config.h>
#include "Spectrum.h"                               
#include "Event.h"
#include "GateContainer.h"
#include "TrueGate.h"
#include <string.h>
#include <iostream>
#include <CAxis.h>
#include <RangeError.h>
#include <stdlib.h>
#include <time.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
static string T("-TRUE-");

static CTrueGate       AlwaysMade;
static CGateContainer  TrueContainer(T, 0, AlwaysMade);
CGateContainer*        pDefaultGate = &TrueContainer;

Bool_t CSpectrum::m_Seedrandom = kfTRUE; // need to init randomizer.


/*!
   Construct a spectrum.
   \param rName (std::string& [in]): Name of the spectrum.
   \param id    (UInt_t [in]):       Id of the spectrum.
   \param Maps  (vector<CAxis> [in]): Set of paramter/axis coordinat maps.
   \param pGate (CGateContainer* [in] default = pDefaultGate):
             pointer to the gate set on the spectrum.  Default is a pointer to
	     a TRUE gate.
*/
CSpectrum::CSpectrum (const std::string& rName,  
		      UInt_t nId,
		      Axes  Maps,
		      CGateContainer* pGate) :
  CNamedItem(rName, nId),
  m_pStorage(0),
  m_fOwnStorage(0),
  m_pGate(pGate),
  m_DataType(keUnknown_dt),	// Up to subclasser to fix this.
  m_AxisMappings(Maps)
{
}

/*!
  Construct a spectrum with no need for parameter axis mapping information.
   \param rName (std::string& [in]): Name of the spectrum.
   \param id    (UInt_t [in]):       Id of the spectrum.
   \param pGate (CGateContainer* [in] default = pDefaultGate):
             pointer to the gate set on the spectrum.  Default is a pointer to
	     a TRUE gate.
*/
CSpectrum:: CSpectrum(const std::string& rName, UInt_t nId,
		      CGateContainer* pGate) :
  CNamedItem(rName, nId),
  m_pStorage(0),
  m_fOwnStorage(0),
  m_pGate(pGate),
  m_DataType(keUnknown_dt)	// Up to subclasser to fix this.
{
}
/*!
  If necessary, the destructor releases the spectrum storage.
*/
CSpectrum::~CSpectrum()
{
  if(m_fOwnStorage) {
    ReleaseStorage();
  }
}

// Functions for class CSpectrum

///////////////////////////////////////////////////////////////////////////
// Function:
//     void operator() (const CEvent& rEvent);
// Operation Type:
//     Evaluator.
//
/*!
   If the spectrum's gate is made, increment the spectrum.
   this default implementation is good for most simple spectra... as only
   the Increment member (also virtual) is spectrum type dependent.
   \param rEvent (const CEvent& [in]) Reference to the event that is being
      evaluated to see how to increment the spectrum.
*/
void
CSpectrum::operator()(const CEvent& rEvent)
{
  // The logic below short circuits the entire gate check if
  // The gate is the default gate. This unintuitive logic is suggested
  // by the results of profiling.

  if((m_pGate == pDefaultGate ) || CheckGate(rEvent)) {
    Increment(rEvent);		// Only increment if gate set.
  }
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CGateContainer* ApplyGate(CGateContainer* pNewGate);
// OPeration Type:
//    Mutator
/*!
  Applies  a new gate to the spectrum, returning the prior gate.CGateContainer*
  \param pNewGate (CGateContainer* [in]): New gate to apply to the spectrum.
  \return pointer to the old gate container applied to the gate.
*/

CGateContainer*
CSpectrum::ApplyGate(CGateContainer* pNewGate)
{


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

/*!
   Replaces the current gate with the TRUE gate, effectively ungating the
   spectrum.
   \return A pointer to the gate that had been set on the spectrum.
*/
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

/*! Instructs the spectrum to move the storage associated
 with its channels into the storage pointed to by the
 parameter.  It is up to the caller to ensure that there
 is sufficient storage in the destination.
 Default action is to:
     memcpy(pNewLoc, m_pStorage, StorageNeeded());
 If the current storage was owned by the spectrum, it is deleted.
 The m_pStorageg and m_fOwnStorage are updated from the parameters.

 Formal Parameters:
      \param  pNewLoc (Address_t [in])
          Pointer to the new spectrum storage.

       \param fTransferOwnership (Bool_t [in] default =  kfTRUE):
           Indicates if the storage should be owned
           by the spectrum after transfer. kfTRUE indicates that the Spectrum
	   will delete the storage on destruction. kfFALSE indicats the storage
	   is externally managed.
*/
void 
CSpectrum::ReplaceStorage(Address_t pNewLoc, Bool_t fTransferOwnership) 
{

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

/*!
 Clears the channels assocaited with the
 spectrum.  By default, this is done via:
   memset(m_pStorage, 0, StorageRequried());
  This function is virtual, however so it can be overridden.

*/ 
void 
CSpectrum::Clear() 
{


  memset(m_pStorage, 0, StorageNeeded());
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Size_t StorageNeeded (  )
//  Operation Type:
//     Selector
//

/*!
 Returns the number of storage units required
 to store the data in the histogram.

   E.g. new UChar_t[StorageNeeded()]]
   could allocate space for the spectrum if it was
   stored uncompressed.,

 Default implementation assumes that the storage is 
 a simple n dimensional array of items.  The only nasty
 bit is figuring out the size of each item, done via a long
 case statement on storage type.
 
If this method does not work for a particular spectrum type. StorageNeeded
is virtual and can be replaced as needed.

 \return Size_t number of bytes of storage required by the spectrum.
*/
Size_t 
CSpectrum::StorageNeeded() const 
{


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
/*!
   Copies the current contents of a spectrum to some destination storage.
   It is the caller's responsibility to ensure that the destination storage 
   is big enough.  This member also requires that StorageNeeded() be accurate.

   Copy is normally used when the spectrum storage is altered.  This can happen
   in the SpecTcl program as a result of an sbind operation to bind the
   spectrum storage to displayer storage.

   \param pStorage (void* [out]): Pointer to the storage into which our
      spectrum will be copied.
*/
void
CSpectrum::Copy(void* pStorage) const
{

  memcpy(pStorage, m_pStorage, StorageNeeded());
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//    Bool_t CheckGate(const CEvent& rEvent)
// Operation Type:
//    Gate check.
//

/*!
   Checks to see if the spectrum gate is satisfied.  This function is
   virtual to allow for 'special' gating requirements to be implemented.

   \param rEvent (CEvent& [in]): Reference to the gate to be checked.
   \return Bool_t kfTRUE if the gate approves the increment of the spectrum.
*/
Bool_t
CSpectrum::CheckGate(const CEvent& rEvent)
{
  return (*m_pGate)((CEvent&)rEvent);
}
/*
  I think this is not necessary in the base class.
void
CSpectrum::GammaGateIncrement(const CEvent& rEvent)
{
  operator()(rEvent);
}
*/



/*!
   Converts an axis coordinate to a raw parameter value on the selected
   axis.
   \param nAxis (UInt_t [in]) Selects the axis number  (0 x 1 y e.g.).
   \param nAxisValue (UInt_t [in]): The axis value to convert.
   \return Float_t  The parameter value corresponding to the axis value.
   \throw CRangeError if nAxis is out of range.
*/
Float_t 
CSpectrum::AxisToParameter(UInt_t nAxis, UInt_t  nAxisValue)
{
  if(nAxis < m_AxisMappings.size()) {
    return m_AxisMappings[nAxis].AxisToParameter(nAxisValue);
  }
  else {
    throw CRangeError(0, m_AxisMappings.size() -1 , nAxis,
		      string("CSpectrum::AxisToParameter"));
  }
}
/*!
   Converts a mapped parameter value to a spectrum axis coordinate.
   \param nAxis (UInt_t [in]) Selects the axis number  (0 x 1 y e.g.).
   \param fParameterValue (Float_t [in]): The parameter value to convert.
   \return UInt_t  The axis coordinate corresponding to fParamterValue.
   \throw CRangeError if nAxis is out of range.
*/
Float_t  
CSpectrum::MappedToAxis(UInt_t nAxis, Float_t fParameterValue)
{
  if(nAxis < m_AxisMappings.size()) {
    return m_AxisMappings[nAxis].MappedParameterToAxis(fParameterValue);
  }
  else {
    throw CRangeError(0, m_AxisMappings.size() - 1, nAxis,
		      string("CSpecrrumParameterToAxis"));
  }
  
}
/*!
   Converts an axis coordinate to a mapped parameter value.
   \param nAxis (UInt_t [in]) Selects the axis number  (0 x 1 y e.g.).
   \param nAxisValue (UInt_t [in]): The axis value to convert.
   \return Float_t  The parameter value corresponding to the axis value.
   \throw CRangeError if nAxis is out of range.
*/
Float_t 
CSpectrum::AxisToMapped(UInt_t nAxis, UInt_t nAxisValue)
{
  if(nAxis < m_AxisMappings.size()) {
    return m_AxisMappings[nAxis].AxisToMappedParameter(nAxisValue);
  }
  else {
    throw CRangeError(0, m_AxisMappings.size() -1 , nAxis,
		      string("CSpectrum::AxisToParameter"));
  }
}
/*!
  Returns the number of dimensions in the spectrum

  \retval UInt_t
     Number of defined axes.
*/
UInt_t 
CSpectrum::Dimensionality() const
{
  return m_nChannels.size();	// Number of registered axes.
}
/*!
  Returns the dimension of a selected axis.  If the axis selected
  is out of range of the allowed axes, zero is returned.

  \param <tt> nDimension (UInt_t [in]) </tt>
      Selects the axis about which we return the information.

  \retval Size_t
     The number of channels on that axis or 0 if the axis doesn't
     exist (no channels).
*/
Size_t
CSpectrum::Dimension(UInt_t nDimension) const
{
  if(nDimension < m_nChannels.size()) {
    return m_nChannels[nDimension];
  } 
  else {
    return 0;
  }
}
/*!
   Returns the low limit value for an axis.

   \param <tt> nDimension (UInt_t [in]): </tt>
      Selects which axis to return information about.

   \retval  Float_t
       The parameter value that corresponds to that axis's channel
       0.

    \throw CRangeError 
        If the nDimensino parameter is not a valid dimension selector.
*/
Float_t
CSpectrum::GetLow(UInt_t nDimension) const
{
  if(nDimension < m_fLows.size() ) {
    return m_fLows[nDimension];
  }
  else {
    throw CRangeError(0, m_fLows.size() - 1, nDimension,
		      string("CSpectrum::GetLow"));
  }
}
/*!
  Returns the high limit value for an axis.

   \param <tt> nDimension (UInt_t [in]): </tt>
      Selects which axis to return information about.

   \retval  Float_t
       The parameter value that corresponds to that axis's last
       channel.

    \throw CRangeError 
        If the nDimension parameter is not a valid dimension 
	selector.
*/
Float_t
CSpectrum::GetHigh(UInt_t nDimension) const
{
  if(nDimension < m_fHighs.size() ) {
    return m_fHighs[nDimension];
  }
  else {
    throw CRangeError(0, m_fHighs.size() - 1, nDimension,
		      string("CSpectrum::GetHigh"));
  }
}
/*!
   Returns a structure that describes the spectrum.
   This can be overridden by subclasses,by default,
   it is filled in from what we know about the 
   spectrum and its axes as a result of construction
   stuff done by the subclasses

*/

CSpectrum::SpectrumDefinition&
CSpectrum::GetDefinition()
{
  
  static SpectrumDefinition Def;
  // Clear the arrays:

  Def.vParameters.erase(Def.vParameters.begin(),
			Def.vParameters.end());
  Def.nChannels.erase(Def.nChannels.begin(),
		      Def.nChannels.end());
  Def.fLows.erase(Def.fLows.begin(),
		  Def.fLows.end());
  Def.fHighs.erase(Def.fHighs.begin(),
		   Def.fHighs.end());

  Def.sName     = getName();
  Def.nId       = getNumber();
  Def.eType     = getSpectrumType();
  Def.eDataType = StorageType();

  GetParameterIds(Def.vParameters);
  for(int i =0; i < Dimensionality(); i ++) {
    Def.nChannels.push_back(Dimension(i));
    Def.fLows.push_back(GetLow(i));
    Def.fHighs.push_back(GetHigh(i));
  }
  //  Def.nChannels = m_nChannels;
  //  Def.fLows     = m_fLows;
  //  Def.fHighs    = m_fHighs;

  return Def;
}
/*!
   Get the units associated with an axis of the spectrum:
   \param <TT> nDimension (UInt_t [in]) </TT>
      The dimension to select.
   \retval string
     Units string (may be empty).
   \throw CRangeError
      If nDimension selects an undefined axis.

*/
string
CSpectrum::GetUnits(UInt_t nDimension) const
{
  if(nDimension < m_Units.size() ) {
    return m_Units[nDimension];
  }
  else {
    throw CRangeError(0, m_Units.size() - 1, nDimension,
		      string("CSpectrum::GetUnits"));
  }
}
/*!
   Randomizes a floating point channel between the two possible integer
   channels.  This is an attempt to de-spike spectra that don't have a good
   uniform sized mapping between the underlying parameter granularity and
   the spectrum granularity.  This won't help when the spectrum granularity
   is finer than the parameter granularity.. then you get channels with
   unconditional zeroes... and that's the experimenter's fault.
  
  This function works like this:
   For  c1 <= f <= c2  let b = (f - c1)  b is in the interval [0,1].
   p(f->c1) = b and p(f->c2) = 1 - b.
*/

Int_t
CSpectrum::Randomize(Float_t channel)
{
  /// FUTURE WORK:  See bug101.
  //    This function needs a lot of future work:
  //    What we really need todo is figure out the range
  //    of channels the parameter can go into and 
  //    then randomize amongst them.. the parameterization
  ///   should have something like:
  //       (value, low,hi) and we randomly place the
  //    value in the interval low,hi.
  //    First pass.. disable all randomization.
  //    Leave real fix as a work item for later.
  //

  return (Int_t)(channel);

  // If necessary seed the randomizer:
  if(m_Seedrandom) {
    time_t seed  = time(NULL);	      // Seed with time of day.
    srand48((long int)seed);
    m_Seedrandom = kfFALSE;
  }
  Int_t low = (Int_t)channel;	      // C1 above. 
  Float_t b = channel - (Float_t)low; // Probability in channel low.

  if(b < (Float_t)drand48()) return low;
  else                       return low+1;
			      
  
}
/*! 
  By default all spectra need a parameter, and the ones that don't can override this.
*/
Bool_t 
CSpectrum::needParameter() const
{
  return kfTRUE;
}

/*---------------------------------------------------------------------------
 * Statistics:
 */

/**
 * createStatArrays
 *   Create new statistics arrays for a spectrum.  Statistics arrays maintain
 *   counters for under/overflow events on each axis.
 *
 * @param nAxes - number of axes desired.
 * @note If the statistics arrays already exist they are first totally detroyed.
 */
void
CSpectrum::createStatArrays(unsigned nAxes)
{
    // If need be destroy the counters:
    if (!m_underflowCounters.empty()) {
      m_underflowCounters.clear();
    }
    if (!m_overflowCounters.empty()) {
         m_overflowCounters.clear();
    }
    
    m_overflowCounters.insert(m_overflowCounters.begin(), nAxes, 0);
    m_underflowCounters.insert(m_underflowCounters.begin(), nAxes, 0);
}
/**
 * clearStatArrays
 *    Replaces all counters with zero.
 *    This is actually done by recreating the arrays using their
 *    current sizes.
 */
void
CSpectrum::clearStatArrays()
{
    size_t nAxes = m_overflowCounters.size();
    createStatArrays(nAxes);               // They start zeroed.
}
/**
 * logOverflow
 *    Increment an overflow counter.
 *
 * @param axis    - Axis number.
 * @param inc     - Amount to increment by (defaults to 1).
 */
void
CSpectrum::logOverflow(unsigned axis, unsigned increment)
{
    m_overflowCounters.at(axis) += increment;
}
/**
 * logUnderflow
 *    Increment an underflow counter.
 *
 * @param axis - axis number.
 * @param inc  - amount to increment by (defaults to 1).
 */
void
CSpectrum::logUnderflow(unsigned axis, unsigned increment)
{
    m_underflowCounters.at(axis) += increment;
}

/**
 * getUnderflows
 *   @return std::vector<unsigned> - Underflow counters.
 */
std::vector<unsigned>
CSpectrum::getUnderflows() const
{
    return m_underflowCounters;
}
/**
 * getOverflows
 *  @return std::vector<unsigned> - overflow counters
 */
std::vector<unsigned>
CSpectrum::getOverflows() const
{
    return m_overflowCounters;
}
