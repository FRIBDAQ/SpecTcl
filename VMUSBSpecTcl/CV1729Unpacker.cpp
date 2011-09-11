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
#include "CV1729Unpacker.h"
#include <Event.h>
#include <stdint.h>
#include <Spectrum.h>
#include <Parameter.h>
#include <SpecTcl.h>
#include <stdio.h>

#include <string>

using namespace std;

static const int numSamples = 2560;

/////////////////////////////////////////////////////////////////////
// Canonicals..

/*!
  Construction is a no-op.

*/
CV1729Unpacker::CV1729Unpacker() {}

/*!
   Destruction is a no-op.
*/
CV1729Unpacker::~CV1729Unpacker() {}

//////////////////////////////////////////////////////////////////////
//  Virtual function overrides.

/**
 * Perform the unpack.  The data format is as follows:
 *  +------------------------------------+
 *  |   Interrupt register. (16)         |
 *  +------------------------------------+
 *  |   Trigger column number            |
 *  +------------------------------------+
 *  |  Digitizer data as defined in      |
 *  |  4.7 of the manual.                |
 *  \                                   /
 *  /                                   \
 *  +------------------------------------+
 *
 *  The interrupt register is 1 if there is data worth looking at.
 *  It is not clear to me yet what the digitizer data looks like if there
 *  is not data.
 * 
 * @param rEvent  - The output event (ignored by this unpacker).
 * @param event   - Raw data from the VM-USB
 * @param offset  - Where in event our data starts.
 * @param pMap    - adc mapping structure that describes this device.
 *                  we care about a couple of things.
 *                  - Using the rEvent indices to locate our parameter base name.
 *                  - Using the parameter basenames to find all 2048 spectra associated
 *                    with each used channel.
 *                  - Using the parammap 'virtual slot' element determine which channels will actually
 *                    provide data.
 * @return unsigned int
 * @retval Value of offset for next part of the event.
 *
 */
unsigned int
CV1729Unpacker::operator()(CEvent&                       rEvent,
			    std::vector<unsigned short>&  event,
			    unsigned int                  offset,
			    CParamMapCommand::AdcMapping* pMap)
{
  Info& info = *(findSpectra(*pMap));
  next(info);
  
  // get the two header words:

  uint16_t irqRegister = event[offset++];
  uint16_t triggerCol  = event[offset++];

  if(irqRegister) {

    // Have data

    // Figure out how many channels we have and which ones from the 'vsn' just in case the
    // user was perverse and defined the wrong parameter names relative to the mask.
    // TOO: Factor this out into findSpectra?

    uint16_t channelMask        = pMap->vsn;
    int      numChannels        = 0;
    bool     channelsPresent[4] = {false, false, false, false};
    for (int i = 0; i < 4; i++) {
      if (channelMask & (1 << i)) {
	channelsPresent[i] = true;
	numChannels++;
      }
    }

    // Skip the first conversions word.

    offset += numChannels;

    // Get the verniers associated with each present channel

    uint16_t verniers[4];
    for (int i =3; i >= 0; i++) {
      if (channelsPresent[i]) {
	verniers[i] = event[offset++];
      }
    }
    // Skip the baseline reset:

    offset += numChannels;

    /* Turn the verniers into the location of the first sample for
       each of the channels present.  Each column represents 20 samples
       the vernier ther sample within the column.  We're assuming the data are read
       down the column.
       When this loop is done verniers + offset is the first channel sample.
       and firstSampleNum is the sample number. for the first sample.
       (used to know when to wrap the buffer.
    */
    int firstSampleNum[4];
    int chan = numChannels;	// offset from sample set.

    for (int i =0; i < 4; i++) {
      if(channelsPresent[i]) {
	firstSampleNum[i] = 20*triggerCol + verniers[i];
	verniers[i] =  firstSampleNum[i] * numChannels + chan;
	chan--;
      }
    }
    // Locate the spectra to fill

    CSpectrum* pSpectra[4];
    for (int i = 0; i < 4; i++) {
      if (channelsPresent[i]) {
	pSpectra[i] = info.s_Spectra[i][info.s_spectrumIndex];
      }
      else {
	pSpectra[i] = 0;
      }
    }

    // Channels are high number to low number.


    for (UInt_t s =0; s < numSamples; s++) {
      for(int i =0; i < 4; i++) {
	if (channelsPresent[i] && pSpectra[i]) {
	  (pSpectra[i])->set(&s, event[firstSampleNum[i]]);
	  firstSampleNum[i] += numChannels;
	  if(firstSampleNum[i] > numSamples*numChannels) {
	    firstSampleNum[i] = 0 + (i-numChannels+1);
	  }
	}

      }
    }

    // Now unpack the data into the first 2048 channels of each spectrum...for each
    // channel.



    // Set the offset correctly.

    offset += numSamples*numChannels;	// There are this many samples.


  }
  else {
    // TODO: Look at data to see how to adjust offset
  }
  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Private utilities:
//

/**
 * locate the pInfo for our mapping element or create it if it does not exist.
 * @param rMap - Reference to the map.
 * @return pInfo
 * @retval pointer to the info.
 */
CV1729Unpacker::pInfo
CV1729Unpacker::findSpectra(CParamMapCommand::AdcMapping& rMap)
{
  SpecTcl* pApi = SpecTcl::getInstance();

  if (! rMap.extraData) {
    pInfo pSpectra = new Info;

    pSpectra->s_spectrumIndex = -1; // start with the first (assuming unpack starts with a next()).
    
    
    // Looping over the channels: 

    for (int chan = 0; chan < 4; chan++) {
      // Null the spectrum pointers:

      for (int spec = 0; spec < 2048; spec++) {
	pSpectra->s_Spectra[chan][spec] = 0;
      } 

      // If the parameter is defined and we can track down its name
      // ..and we can track down the spectra associated with it, put pointers to
      // them into the struct.

      if (rMap.map[chan] != -1) {
	CParameter* pParam = pApi->FindParameter(rMap.map[chan]);
	if (pParam) {
	  string baseName = pParam->getName();
	  for (int i = 0; i < 2048; i++) {
	    char spectrumName[2048];
	    snprintf(spectrumName, sizeof(spectrumName), "%s.%04d", baseName.c_str(), i);
	    CSpectrum* pSpec = pApi->FindSpectrum(spectrumName);
	    pSpectra->s_Spectra[chan][i] = pSpec; // Correct thing to do even if there's no match.
	  }

	}
      }
    }

    rMap.extraData = pSpectra;

  }

  return reinterpret_cast<pInfo>(rMap.extraData);
}
/**
 * Manages the spectrumIndex in the info struct; Moves to the next spectrum
 * - Increments s_spectrumIndex
 * - If s_spectrumIndex >= 2048 clears all spectra and sets itself to 0.
 *
 * @param rInfo - reference to the rInfo variable.
 */
void
CV1729Unpacker::next(CV1729Unpacker::Info& rInfo)
{
  rInfo.s_spectrumIndex++;
  if (rInfo.s_spectrumIndex >= 2048) { // Need to reset/clear.
    rInfo.s_spectrumIndex = 0;
    for (int c = 0; c < 4; c++) {
      for (int s = 0; s < 2048; s++) {
	if (rInfo.s_Spectra[c][s]) {
	  //	  rInfo.s_Spectra[c][s]->Clear(); // For now don't clear..
	}

      }
    }

  }
}
