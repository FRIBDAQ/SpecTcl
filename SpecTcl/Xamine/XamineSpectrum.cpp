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


//
// Header Files:
//
#include <config.h>
#include "XamineSpectrum.h"                               
#include <string.h>
#include <assert.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Functions for class CXamineSpectrum

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t is1d (  )
//  Operation Type:
//     selector
//
Bool_t 
CXamineSpectrum::is1d() 
{
  spec_type st = getSpectrumType();
  switch(st) {
  case onedlong:
  case onedword:
    return kfTRUE;
  case twodword:
  case twodlong:
  case twodbyte:
  case undefined:
    return kfFALSE;
  default:
    assert(kfFALSE);		// Should be legal value here.
  }
  assert(0);			// Should not get here either.
  return kfFALSE;
}
///////////////////////////////////////////////////////////////////////////
// 
// Function:
//    CXamineSpectrum(volatile Xamine_shared* pXamine, UInt_t nSlot)
// Operation Type:
//    Parameterized constructor.
//
CXamineSpectrum::CXamineSpectrum(volatile Xamine_shared* pXamine, UInt_t nSlot) :
  m_pStorage(0),
  m_nSlot(nSlot),
  m_pXamineMemory(pXamine)
{
  // From the slot and base pointer figure out where the storage is and
  // also pick up the title string.  No assumptions are made about the blank
  // or null filledness of the titles in Xamine's memory.
  //
  UInt_t nOffset = m_pXamineMemory->dsp_offsets[m_nSlot];

  switch(m_pXamineMemory->dsp_types[m_nSlot]) { 
  case twodlong:
  case onedlong:		// Scale the offset appropriately.
    nOffset = nOffset*sizeof(Int_t);
    break;
  case onedword:
  case twodword:
    nOffset = nOffset*sizeof(Short_t);
    break;
  case twodbyte:
    break;
  default:			// Spectrum undefined...
    return;			// Don't do anything else.
  }
  m_pStorage = (Address_t)(m_pXamineMemory->dsp_spectra.XAMINE_b + nOffset);

  char szTitle[sizeof(spec_title) + 1];
  memset(szTitle, 0, sizeof(spec_title)+1);
  strncpy(szTitle, 
	  (char*)m_pXamineMemory->dsp_titles[m_nSlot],
	  sizeof(spec_title));	// Ensured Null terminated string...
  m_sTitle = szTitle;		// Which can be set to m_sTitle.

}













