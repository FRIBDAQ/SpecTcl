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
#include "CRateProcessor.h"
#include <Spectrum.h>
#include <Analyzer.h>
#include <BufferDecoder.h>
#include <SpecTcl.h>
#include "CRateList.h"
#include <iostream>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

////////////////////////////////////////////////////////////////////////////
/////////////////  Canonical functions custructors etc /////////////////////
////////////////////////////////////////////////////////////////////////////

/*!
   Normal creation.
   \param spectrum  : CSpectrum &
       Reference to a spectrum for which totals/increments will be maintained.
*/

CRateProcessor::CRateProcessor(CSpectrum& spectrum) :
  m_pSpectrum(&spectrum),
  m_totalCounts(0),
  m_increments(0)
{}

/*!
   Copy construction
*/
CRateProcessor::CRateProcessor(const CRateProcessor& rhs) :
  m_pSpectrum(rhs.m_pSpectrum),
  m_totalCounts(rhs.m_totalCounts),
  m_increments(rhs.m_increments)
{
}

/*!
  Destructor:
*/
CRateProcessor::~CRateProcessor() {}

/*!
   Assignment is so like copy construction.
*/
CRateProcessor&
CRateProcessor::operator=(const CRateProcessor& rhs)
{
  if(this != &rhs) {
    CEventProcessor::operator=(rhs);
    m_pSpectrum   = rhs.m_pSpectrum;
    m_totalCounts = rhs.m_totalCounts;
    m_increments  = rhs.m_increments;
  }
  return *this;
}
/*!
   comparison.. all elements equal -> equal.
*/
int
CRateProcessor::operator==(const CRateProcessor& rhs)
{
  return (CEventProcessor::operator==(rhs)                  &&
	  (m_pSpectrum   == rhs.m_pSpectrum)                &&
	  (m_totalCounts == rhs.m_totalCounts)              &&
	  (m_increments  == rhs.m_increments));
}
/*!
  Comparison != is !(==).
*/
int
CRateProcessor::operator!=(const CRateProcessor& rhs) 
{
  return !(*this == rhs);
}

/////////////////////////////////////////////////////////////////////////
//////////////////////// Selectors. /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/*!
   Return a pointer to the spectrum.  This allows the caller
   to e.g. get the name of the spectrum.
*/
CSpectrum*
CRateProcessor::getSpectrum()
{
  return m_pSpectrum;
}
/*!
   Return the total counts value.
*/
uint64_t
CRateProcessor::getTotals() const
{
  return m_totalCounts;
}
/*!
  Return the increments value.
*/
uint64_t
CRateProcessor::getIncrements() const
{
  return m_increments;
}

/////////////////////////////////////////////////////////////////////////
///////////////////////// Virtual function overrides ///////////////////
////////////////////////////////////////////////////////////////////////


/*!
   Process begin run buffer.  We ensure the spectrum still exists or, if necessary,
   we update the pointer with the new instance of the spectrum.

   \param analyzer : CAnalyzer& 
       Reference to the analyzer.
    \param decoder : CBufferDecoder&
        Reference to the buffer decoder.
*/

Bool_t
CRateProcessor::OnBegin(CAnalyzer& analyzer, CBufferDecoder& decoder)
{
  adjustSpectrum();
  if (!m_pSpectrum) {		// disabled...
    return kfTRUE;
  }

  // Zero our counters.

  clear();

  return kfTRUE;
}

/*!
   Process buffers that are 'interesting'  In our case these are scaler
   buffers as they have the timebase information that can be used
   later on to calculate rates.  If the buffer is a scaler buffer 
   >and< if the spectrum still exists, calculate the new totals/increments.
   \param analyzer : CAnalyzer&
       The analyzer director object.
   \param decoder : CBufferDecoder&
       The buffer decoder for this case.

*/

Bool_t
CRateProcessor::OnOther(UInt_t nType, CAnalyzer& analyzer, CBufferDecoder& decoder)
{
  if (nType != 2) {
    return kfTRUE;		// Not a scaler.
  }

  adjustSpectrum();
  if (!m_pSpectrum) {
    return kfTRUE;		//  No longer have a spectrum.
  }

  // Now sum based on the spectrum dimensionality:

  UInt_t numDims = m_pSpectrum->Dimensionality();
  uint64_t sum;

  if (numDims == 1) {
    sum = sum1d();
  }
  else if (numDims == 2) {
    sum = sum2d();
  }
  else {
    // Bad spectrum!!!
    cerr << "Warning-- spectrum dimensionality invalid disabling rates for "
	 << m_pSpectrum->getName() << endl;
    disable();
    return kfTRUE;
  }
  m_increments  = sum - m_totalCounts;
  m_totalCounts = sum;

  return kfTRUE;
}
////////////////////////////////////////////////////////////////////
///////////////////// Private utility functions ////////////////////
////////////////////////////////////////////////////////////////////


/* disable ourself. */

void
CRateProcessor::disable()
{
    CRateList* pList = CRateList::getInstance();
    pList->markForDeletion(*this);
    m_pSpectrum == static_cast<CSpectrum*>(NULL);
}

/* Update the spectrum pointer if needed. */
void
CRateProcessor::adjustSpectrum()
{
  // Ensure the spectrum still exists.

  SpecTcl& api(*(SpecTcl::getInstance()));

  string spectrumName = m_pSpectrum->getName();
  CSpectrum* match    = api.FindSpectrum(spectrumName);

  // If the spectrum is gone, mark ourselves for deletion and disable 
  // by setting the spectrum to null.

  if (!match) {
    disable();
    return;
  }
  // If the spectrum changed, then update our pointer.
  // and reset the counters:

  if (match != m_pSpectrum) {
    m_pSpectrum = match;
    clear();
  }
}

/*
   Clear the increments/totals.
*/
void
CRateProcessor::clear()
{
  m_increments  = 0;
  m_totalCounts = 0;
}
/*   Update the rates for a 1-d spectrum.  */

uint64_t 
CRateProcessor::sum1d()
{
  UInt_t    dim = m_pSpectrum->Dimension(0);
  uint64_t  sum = 0;

  for(UInt_t i =0; i < dim; i++) {
    sum += (*m_pSpectrum)[&i];
  }
  return sum;
}
/*    Update rates for 2-d spectrum */

uint64_t
CRateProcessor::sum2d()
{
  UInt_t dimx  = m_pSpectrum->Dimension(0);
  UInt_t dimy  = m_pSpectrum->Dimension(1);
  uint64_t sum = 0;
  for (int y = 0; y < dimy; y++) {
    for(int x = 0; x < dimx; x++) {
      UInt_t indices[2];
      indices[0] = x;
      indices[1] = y;
      
      sum += (*m_pSpectrum)[indices];
    }
  }
  return sum;
}
