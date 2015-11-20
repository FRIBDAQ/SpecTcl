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

#ifndef _CSPECTCLSPECTRUMALLOCATOR_H
#define _CSPECTCLSPECTRUMALLOCATOR_H
/**
 * @file CSpecTclSpectrumAllocator.h
 * @brief Class definition for the SpecTcl spectrum allocator.
 */


#ifndef _CSPECTRUMALLOCATOR_H
#include "CSpectrumAllocator.h"
#endif

#ifndef __STDLIB_H
#include <stdlib.h>
#ifndef __STDLIB_H
#define __STDLIB_H
#endif
#endif

class CSpectrum;

/**
 * @class CSpecTclSpectrumAllocator
 *
 *  SpecTcl spectrum allocator for both 1d and 2d spectra.
 */
class CSpecTclSpectrumAllocator : public CSpectrumAllocator
{
  // The data here is maintained for testing purposes:
private:
  size_t m_xDimension;
  size_t m_yDimension;
  size_t m_bytesAllocated;
  bool   m_allocated;

  // Canonicals

public:
  CSpecTclSpectrumAllocator();

  void* allocate(CSpectrum& spec);
  void  deallocate(CSpectrum& spec);
};
#endif
