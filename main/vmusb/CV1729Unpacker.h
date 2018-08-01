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

#ifndef CV1729UNPACKER_H
#define CV1729UNPACKER_H

#include "CModuleUnpacker.h"


class CSpectrum;

/*!
  This unpacker is responsible for unpacking 
  data from CAEN V1729 FADC waveform digitizers.
  Each defined channel is mapped to a set of pre-created
  2k spectra.  waveforms get put into each spectrum until all spectra
  are used up at which point the spectra are all cleared and our antics start
  all over again.  The slot in the parameterm ap that's normally reserved for a virtual slot
  instead contains the mask of channels in use by this module.
 
  A current restriction is that the number of columns read is always 256.
*/
class CV1729Unpacker : public CModuleUnpacker
{
  // private data structures:
private:
  typedef struct _info {
    int        s_spectrumIndex;	   // Which spectrum to fill next.
    CSpectrum* s_Spectra[4][2048]; // Spectrum pointers for each channel.
  } Info, *pInfo;

public:
  // Canonicals:

  CV1729Unpacker();
  virtual ~CV1729Unpacker();

  // The unpacker entry:

public:
  virtual unsigned int operator()(CEvent&                       rEvent,
				  std::vector<unsigned short>&  event,
				  unsigned int                  offset,
				  CParamMapCommand::AdcMapping* pMap);

  // Private utilities:

private:
  
  pInfo findSpectra(CParamMapCommand::AdcMapping& rMap);
  void  next(Info& rInfo);

};

#endif
