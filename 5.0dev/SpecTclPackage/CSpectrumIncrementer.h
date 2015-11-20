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
#ifndef _CSPECTRUMINCREMENTER_H
#define _CSPECTRUMINCREMENTER_H




/**
 * @file CSpectrumIncrementer.h
 * @brief ABC For class that understands underlying spectra.
 */


#ifndef _STDINT_H
#include <stdint.h>
#ifndef _STDINT_H
#define _STDINT_H
#endif
#endif

class CSpectrum;

/**
 * @class CSpectrumIncrementer
 *
 *  See the CSpectrum class information for more details
 *  on why/how this all works.  Ths class is an abstract
 *  base class that understands how to interact with underlying
 *  spectra from a specific histogramming kernel for a specific
 *  SpecTcl Spectrum type.  See the comments in the
 *  interface methods below for more information about the
 *  expectations for each method.
 *
 *  Note that typically there will be a family of these for each
 *  histogramming kernel, one for each type of SpecTcl spectrum.
 */
class CSpectrumIncrementer {
public:
  /**
   * Typically the spectrum incrementer will know about some set
   * of SpecTcl parameters.  This method is hooked into an observer
   * for the SpecTcl parameter dictionary and will get called if the
   * dictionary indicates a parameter has been deleted or created.  At that point,
   * the incrementer must determine if the parameters it needs to function
   * are defined and locate them so that it can perform its event by event
   * work.
   *
   * @param spec - spectrum reference.
   */
  virtual void validate(CSpectrum& spec) = 0;
  /**
   * Called for each event in the data if the histogram condition
   * returned true for this event.  This method must determine which channels
   * (if any) get incremented by this event and do the increment.
   *
   * @param spec - spectrum reference.
   */
  virtual void operator()(CSpectrum& spec) = 0;
  /**
   * Expected to retrieve the value of the specified channel of the spectrum
   * (spectrum channel coordinates) from the underlying spectrum.
   *
   * @param spec - spectrum reference.
   * @param x    - X channel number to retrieve.
   * @param y    - Y channel number to retrieve, ignore this if the underlying
   *               spectrum is 1-D.
   * 
   * @return uint32_t - The value from that spectrum channel.
   */
  virtual uint32_t get(CSpectrum& spec, unsigned x, unsigned y) = 0;
  /**
   * Expected to set a spectrum channel to a specific value.
   * Coordinates are in channel coordinates.
   *
   * @param spec - Spectrum reference
   * @param x    - X Channel of the channel to retrieve.
   * @param y    - Y Channel of the channel to retrieve.
   * @param value - Value to which the parameter will be set.
   */
  virtual void set(CSpectrum& spec, unsigned x, unsigned y, uint32_t value) = 0;
  /**
   * Expected to clear all channels of the underling spectrum to zero.
   * 
   * @param spec - the spectrum reference.
   */
  virtual void clear(CSpectrum& spec) = 0;

  // services for subclasses:

protected:
  static void badChannel(unsigned ch, unsigned nch);
};

#endif
