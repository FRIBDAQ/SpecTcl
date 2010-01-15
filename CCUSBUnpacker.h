#ifndef __CCUSBUNPACKER_H
#define __CCUSBUNPACKER_H
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


#ifndef __EVENTPROCESSOR_H
#include <EventProcessor.h>
#endif


/*!
    Unpack data from the CCUSB when it contains a bunch of 
    data from Phillips modules.  All phillips modules are assumed
    to be configured to at least insert a channel hit pattern  and are
    in zero suppressed mode.  This implies that the hit pattern tells us how
    many channels are present in the data.  Each channel includes a channel number tag
    in the top 4 bits that, along with the position of the module in the data stream
    unambiguously determines the channel.

    The parammap command is used to describe the correspondence between
    module/channel and SpecTcl parameters.  We can fetch from this map from the
    parammap command singleton in terms of parameter ids which makes the unpacking
    of a module relatively trivial.

    The format of a single event is:
\verbatim

  +-------------------+
  | event word count  |
  +-------------------+
  |  id for first mod |
  +-------------------+
  | Data from first   |
  |  module           |
  |   /\/\/\/\/\
  +-------------------+
  | id for second mod |
    etc.

\endverbatim

\note the event word count is not self inclusive

*/

class CCUSBUnpacker : public CEventProcessor
{
public:
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder);



};


#endif
