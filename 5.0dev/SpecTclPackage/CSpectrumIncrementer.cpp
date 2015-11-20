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
#include "CSpectrumIncrementer.h"
#include <stdexcept>
#include <sstream>

/**
 * badChannel
 *
 *  Throws a range exception for a bad channel value.
 *
 * @param ch - The channel number that's bad.
 * @param nch - Number of channels in the spectrum.
 */
void
CSpectrumIncrementer::badChannel(unsigned ch, unsigned nch)
{
    std::stringstream os;
    os << "Invalid channel number " << ch << " must be in the range [0.."
       << (nch - 1) << "]";
    throw std::out_of_range(os.str());
}
