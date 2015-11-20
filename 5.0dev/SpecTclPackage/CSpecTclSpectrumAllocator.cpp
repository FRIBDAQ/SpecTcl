/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2013.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


/**
 * @file CSpecTclSpectrumAllocator.h
 * @brief Implementation of the spectrum allocator for SpecTcl spectra.
 */


#include "CSpecTclSpectrumAllocator.h"
#include "CSpectrum.h"
#include "CAxis.h"
#include <stdlib.h>

/**
 * constructor
 *
 *  Just set the member data so that the tests can peek into what we think
 *  we did.
 */
CSpecTclSpectrumAllocator::CSpecTclSpectrumAllocator() : 
  m_xDimension(0), m_yDimension(0), 
  m_bytesAllocated(0), m_allocated(false) 
{
}
/**
 * allocate
 *
 * Analyze the spectrum axes to know the storage requirements
 * and allocate the spectrum storage.
 * All spectra will be uint32_t channels.
 *
 * @param spec - Reference to the spectrum object we are attached to.
 *
 * @return void* - in this case a pointer to the storage allocated.
 */
void*
CSpecTclSpectrumAllocator::allocate(CSpectrum& spec)
{
  // assume it's 1-d and modify the assumption accordingly:

  m_yDimension = 1;
  CAxis* x = spec.getAxis(0);
  m_xDimension = x->channels();

  CAxis* y = spec.getAxis(1);
  if (y) {
    m_yDimension = y->channels();
  }

  // total storage:

  m_bytesAllocated = m_xDimension * m_yDimension * sizeof(uint32_t);

  void* pData = malloc(m_bytesAllocated);
  m_allocated = true;

  return pData;
}
/**
 * deallocate 
 *
 *  deallocate storage for the spectrum's data.
 *
 * @param spec - reference to the spectrum.
 */
void
CSpecTclSpectrumAllocator::deallocate(CSpectrum& spec)
{
  void* pData = spec.getHandle();
  free(pData);
  m_allocated = false;
}
