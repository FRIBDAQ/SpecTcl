//  CXamineSpectrumIterator.cpp
// This iterator allows iteration through the Xamine
// spectrum shared memory picking off the definitions
// of spectra which are defined sequentially.
// Note: Late binding is used for the m_pSpectrum
// member.
//  Note also that operator++ will skip over the
// undefined spectra, and that dereferences of the
// end position iterator are undefined.
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

#include "XamineSpectrumIterator.h"                               
#include "XamineSpectrum.h"
#include "Xamine1D.h"
#include "Xamine2D.h"

static const char* Copyright = 
"CXamineSpectrumIterator.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CXamineSpectrumIterator

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CXamineSpectrum* operator* (  )
//  Operation Type:
//     Selector.
//
CXamineSpectrum& 
CXamineSpectrumIterator::operator*() 
{
// Late binds to and returns reference to *m_pSpectrum
// Note that if m_nSlot == XAMINE_MAXSPEC, this operator
// is not defined.

  bind();
  return *m_pSpectrum;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CXamineSpectrumIterator& operator++ (  )
//  Operation Type:
//     Autoinc 
//
CXamineSpectrumIterator& 
CXamineSpectrumIterator::operator++() // Preincrement
{
// Increments m_nSlot to the next used spectrum slot
// or alternatively to XAMINE_MAXSPEC if there are no
// more used spectra.
// Returns reference to this.  Both preincrement (operator++()) and
// postincrement (operator++(int)) exist. Postincrement returns  a copy
// of this and then increments.
//
  unbind();
  next();
  return *this;

}

CXamineSpectrumIterator
CXamineSpectrumIterator::operator++(int) //   Post incr.
{
  CXamineSpectrumIterator copy(*this);
  next();
  unbind();
  return copy;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator!= ( const CXamineSpectrum& rRhs )
//  Operation Type:
//     
//
int 
CXamineSpectrumIterator::operator!=(const CXamineSpectrumIterator& rRhs) 
{


  return (!operator==(rRhs));
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CXamineSpectrumIterator* operator-> (  )
//  Operation Type:
//     ptr referencing.
//
CXamineSpectrum* 
CXamineSpectrumIterator::operator->() 
{

  bind();
  return m_pSpectrum;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void bind (  )
//  Operation Type:
//     Utility
//
void 
CXamineSpectrumIterator::bind() 
{
// Binds m_pSpectrum to an appropriate CXamineSpectrum defined
// dynamically allocated object.
//

  if(!m_pSpectrum) {	    	      // Never rebind.
    if(m_nSlot < XAMINE_MAXSPEC) {    // Never bind end(). 
      CXamineSpectrum spec(m_pXamineMemory, m_nSlot);
      if(spec.is1d()) {
	m_pSpectrum = new CXamine1D(m_pXamineMemory, m_nSlot);
      }
      else {
	m_pSpectrum - new CXamine2D(m_pXamineMemory, m_nSlot);
      }
    }
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void unbind (  )
//  Operation Type:
//     Utility
//
void 
CXamineSpectrumIterator::unbind() 
{
// Unbinds m_pSpectrum.
// Exceptions:  

  delete m_pSpectrum;
  m_pSpectrum = 0;		// mark unbound too.
}
//////////////////////////////////////////////////////////////////////////
//  
// Function:
//     void next()
// Operation Type:
//     Protected utility:
//
void
CXamineSpectrumIterator::next()
{
  // Sets m_nSlot to the next used spsectrum.

  while(m_nSlot < XAMINE_MAXSPEC) {
    m_nSlot++;
    if(m_nSlot < XAMINE_MAXSPEC) 
      if(m_pXamineMemory->dsp_types[m_nSlot] != undefined)
	break;
  }

}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   void DoAssign(const CXamineSpectrumIterator& rhs)
// Operation type:
//   Assignment/copy utility function.
//
void 
CXamineSpectrumIterator::DoAssign(const CXamineSpectrumIterator& rhs)
{
  m_pXamineMemory = rhs.m_pXamineMemory;
  m_nSlot         = rhs.m_nSlot;
  m_pSpectrum     = 0;		// Unbound.
}
///////////////////////////////////////////////////////////////////////
//
// Function:
//    CXamineSpectrumIterator(Xamine_shared* pXamine,
//                            UInt_t nSLot = 0)
// Operation type:
//    Constructor
//
CXamineSpectrumIterator::CXamineSpectrumIterator(Xamine_shared* pXamine,
						 UInt_t nSlot) :
  m_pXamineMemory(pXamine),
  m_nSlot(nSlot),
  m_pSpectrum(0)
{
  //
  // Locate first used one from nslot.

  if(pXamine->dsp_types[nSlot] == undefined) next();
  
}
