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

#ifndef _CSPECTRUMALLOCATOR_H
#define _CSPECTRUMALLOCATOR_h

/**
 * @file CSpectrumAllocator.h
 * @brief ABC for class that creates underlying spectra.
 */

class CSpectrum;

/**
 * @class CSpectrumAllocator
 *
 *  See CSpectrum class information for more details on why/how
 *  this all works.   This class is an abstract base class
 *  for the storage management for spectra from a specific
 *  histogramming kernel.   It must create the actual
 *  spectrum from the underlying kernel and return an abstract
 *  handle.  It must also know how to destroy the underlying
 *  spectrum.
 *  
 *  The comments for each interface method below hopefully
 *  make this more clear.
 */
class CSpectrumAllocator {
public:
  /**
   * Concrete classes must implement this function such
   * that when called it produces an underlying spectrum of the
   * appropriate type and shape and returns an opaque handle to it.
   * For example a root allocator might produce a TH1L and return a 
   * pointer to it.
   *
   * @param spec - Reference to the owning spectrum.  At this time,
   *              the spectrum description has been filled out and
   *              can be queried for its name, and axis specifications.
   */
  virtual void* allocate(CSpectrum& spec)  = 0;
  /**
   * Concrete classes must implement this function such that when called,
   * The deallocate the underlying histogram type.  Note that each allocator
   * may be called on to allocate for several spectra.  This means that 
   * you cannot use smart-pointer techniques (delete the underlying spectrum
   * in your destructor).  What you must do is use your reference to the
   * spectrum to get the old opaque pointer back and any other information
   * you need from the spectrum and internal data you maintain and 
   * use that to determine how to release the storage.
   *
   * @param spec - Reference to the owning spectrum.  At this time all
   *              entries that describe the spectrum are still valid.
   *              as, of course, the handle produced by allocate.
   */

  virtual void deallocate(CSpectrum& spec) = 0;
};


#endif
