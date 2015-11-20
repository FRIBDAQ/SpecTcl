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

#ifndef _CSPECTRUM_H
#define _CSPECTRUM_H

/**
 * @file CSpectrum.h
 * @brief Aggregation class that pulls together all spectrum components.
 */

#ifndef _STDINT_H
#include <stdint.h>
#ifndef _STDINT_H
#define _STDINT_H
#endif
#endif

#ifndef _STD_VECTOR
#include <vector>
#ifndef _STD_VECTOR
#define _STD_VECTOR
#endif
#endif

#ifndef _STD_STRING
#include <string>
#ifndef _STD_STRING
#define _STD_STRING
#endif
#endif


class CAxis;
class CCondition;
class CSpectrumIncrementer;
class CSpectrumAllocator;

/**
 * @class CSpectrum
 *
 *   This class aggregates the components that make up a spectrum into
 *   a single set of entry point methods.  A spectrum is composed of the
 *   following:
 *   - At least on CAxis object that defines the spectrum axes and how
 *     parameter values map to the binning of that axis.
 *   - A CCondition which must evaluate to true for an event if that
 *     event can increment the spectrum.
 *   - A CSpectrumAllocator which is responsible for interacting with the
 *     histogramming core used by the spectrum to create and delete
 *     spectra in that core (for example creating and deleting TH1L
 *     objects if the histograming core is root.
 *   - A CSpectrumIncrementer that is really more responsible for
 *     interacting with the histogram core and the histogram create by
 *     the allocator for this histogram.  This object is usually
 *     not only specialized to the histogramming core but also to the
 *     SpecTcl type of spectrum.  For example, a SpecTcl summary spectrum
 *     would be realized by a TH2L spectrum in a root histogramming core
 *     and the incrementer would increment multiple channels in that spectrum
 *     for each event ..because that's what makes it a summary spectrum.
 *
 *  CSpectrum needs these components but is neutral about how they come into
 *  being, only insisting that they be passed as construction parameters.
 *
 *  Here are a few examples of how this all interacts.
 *
 *  - Construction:  The CSpectrum object initializes all of the data that
 *    describes the spectrum and then asks its allocator to create the spectrum.
 *    The allocator queries back to the spectrum object to get axis definitions
 *    and possibly the name, and then creates the appropriate spectrum in the
 *    underlying histogramming core (e.g. a TH1L or TH2L for root).  A handle
 *    to this underlying spectrum is passed back and stored in CSpectrum for
 *    retrieval by other components that need it.  The handle is often but not
 *    necessarily a direct pointer to the created object.
 *  - Increment.  The CSpectrum object invokes it's CConditions's operator()
 *    which returns true if the condition was made.   CSpectrum then invokes
 *    operator() of it's CSpectrumIncrementer (hence the name) and that object
 *    is supposed to determine which channels to increment and to do that
 *    for the underlying histogramming core.
 *
 *  While CSpectrum is neutral on this, one could imagine a two level hierarchy of
 *  CSpectrumIncrementers.  The top leve would determine which channels of a histogram
 *  need to be incremented etc. while the bottom level would be responsible for
 *  knowing how to apply those increments to the appropriate spectrum type.
 *
 * @note With the exception of the allocator which is assumed to be shareable
 *       amongst serveral spectrra, all components are assumed to be 
 *       dynamically allocated and their storage, once CSpectrum is constructed,
 *       is managed by CSpectrum.
 */
class CSpectrum
{
  // Data:
private:
  void*                 m_allocationHandle; // Spectrum handle from the allocator.
  std::string           m_name;
  std::vector<CAxis*>   m_axes;	            // Axes descriptions.
  CCondition*           m_condition;        // Condition applied to spectrum.
  CSpectrumIncrementer* m_incrementer;	   
  CSpectrumAllocator*   m_allocator;

  // canonicals:

public:
  CSpectrum(const char* pName, 
	    CSpectrumAllocator* pAllocator, 
	    CSpectrumIncrementer* pIncrementer,
	    CCondition* pCondition, std::vector<CAxis*> axes);
  
  virtual ~CSpectrum();
  
  // Selectors the components will need:
public:
  void* getHandle();
  std::string getName() const;
  CAxis* getAxis(unsigned which);
  CCondition* getCondition();
  CSpectrumIncrementer* getIncrementer();
  CSpectrumAllocator*   getAllocator();

  // Conditions can change:

  void setCondition(CCondition* pNewCondition);
  
  // These methods pass on to component methods or combinations of them.

public:
  void clear();
  void operator()();
  uint32_t get(unsigned x, unsigned y);
  void set(unsigned x, unsigned y, uint32_t value);
  void checkValidity();
};


#endif
