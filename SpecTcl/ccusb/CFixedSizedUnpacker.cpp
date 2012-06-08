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
#include "CFixedSizedUnpacker.h"
#include <Event.h>
#include <vector>
#include <stdint.h>

using namespace std;

/*!
  Construction saves the number of data words in the payload section:
  @param numWords - Number of words in the data payload.
*/
CFixedSizedUnpacker::CFixedSizedUnpacker(int numWords) :
  m_numWords(numWords) {}

/*!
   Unpack a block of data that looks like:

\verbatim
    +-----------------------+
    |   id                  |
    +-----------------------+
    | Channel 0 data        |
    +-----------------------+
    ...  .                 ...
    |  Channel n data       |
    +-----------------------+
\endverbatim

  @param p  - Translator pointer that gets us at the data
              (the id is presumed to have been previously validated).
  @param pModuleInfo - Pointer to the module info (which includes the
               channel-> parameter map.
  @param rEvent - Reference to the channel -> parametr map.
  @return int
  @retval Number of words actually consumed (1 + m_numWords)

*/
int
CFixedSizedUnpacker::unpack(TranslatorPointer<UShort_t> p,
			    const CParamMapCommand::ParameterMap* pModuleInfo,
			    CEvent& rEvent)
{
  ++p;
  const vector<int>& parameterMap = pModuleInfo->s_parameterIds;

  for (int chan =0; chan < m_numWords; chan++) {
    uint16_t datum = *p; ++p;
    if (parameterMap.size() > chan) {
      int param = parameterMap[chan];
      if (param >= 0) {
	rEvent[param] = datum;
      }
    }
  }

  return m_numWords + 1;
}
