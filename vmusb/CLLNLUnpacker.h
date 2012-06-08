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

#ifndef __CLLNLUNPACKER_H
#define __CLLNLUNPACKER_h

#ifndef __EVENTPROCESSOR_H
#include <EventProcessor.h>
#endif

#ifndef __TRANSLATORPOINTER_H
#include <TranslatorPointer.h>
#endif


#ifndef __STL_LIST
#include <list>
#ifndef __STL_LIST
#define __STL_LIST
#endif
#endif


#ifndef __CRT_STDINT
#include <stdint.h>
#ifndef __CRT_STDINT
#define __CRT_STDINT
#endif
#endif

/*!
  This class unpacks data from the LLNL neutron data taking system
  into parameters.  Events have a format that is described in
  the VM-USB manual at about page 29, and is reproduced here.
  Events come in fragments that are at most 2K words long.
  A fragment has a header that looks like this:
\verbatim
  15-13          12            11 - 0
  +-----------+-------------+--------------------+
  | stackid   | continued   |  fragment_length   |
  +-----------+-------------+--------------------+

\endverbatim
  Where:
  - stackid is the id of the stack that produced the event. For us this is
    always 2.
  - continued indicates this is not the last fragment of the event.
  - fragment_length is the number of 16 bit words in the event.

  Following each event fragment is a word of 0x5555, that is not counted
  in the fragment_length.

  The LLNL Neutron data is essentially a singles data taking system. Each
  detector has an energy and TAC singal, but no coincidences between detectors
  are not meaningful.  Data taking, however will lump the data from several
  triggers into a single VM-USB event for efficiency sake... it is very possible,
  therefore, that a VMUSB event will contain data from the same adc channels more 
  than once, in fact we are hoping it will.

  Processing therefore follows a state machine.
  States are:
\verbatim
  Initial - We don't know the length of the current fragment. the next word
            will either be the 0x5555 (invalid header for us since our stack id
            is 2), or a fragment header. We extract the fragment length
            and enter the Internal state.
  Internal- We are in the middle of an event fragment.  In this state, the next
            longword should be the header of an ADC.  We unpack ADC and there are
            three potential cases:
            o ADC falls completely within the event fragment, we remain in the
              Internal state.
            o ADC completes the event fragment, we enter the Initial state.
            o ADC Spans the event fragment boundary. We skip the 0x5555, decode
              the next fragment length and continue processing the event...the event
              end condition will then be one of the previous 2 cases (since a single
              ADC header/data/trailer is at most 34longs it will not require more
              than the end of one fragment and the beginning of another.

  We have a nasty side issue to deal with.. That is that the number of events
in the buffer header is really a bit of a fake.  The number of events listed in the
buffer header is actually the number of VM-USB events, and we are going
to process more events than that.. given how we operate.  We are therefore
going to use a feature of SpecTcl-3.2 that is not available in earlier versions.
SpecTcl-3.2 will add a member function to CAnalyzer called:
entityNotDone().  We will invoke this for every event until we reach the
end of a full event (that is the end of a segment with the continuation bit
not set).  This will prevent the analyzer from decrementing the event count
which would make us analyze too little of the buffer.

\endverbatim
  
 */
class CLLNLUnpacker : public CEventProcessor
{
  // Private data types.

private:
  typedef enum _UnpackState {
    Initial,
    Internal
  } UnpackState;

  // member data:
private:
  UnpackState         m_state;	      // Current state of unpacker.
  STD(list)<uint16_t> m_event;	      // Super event data.
  UInt_t              m_size;	      // Full size of superevent (words).


public:
  // Canonicals:  
  CLLNLUnpacker();
  CLLNLUnpacker(const CLLNLUnpacker& rhs);
  virtual ~CLLNLUnpacker();
 
  CLLNLUnpacker& operator=(const CLLNLUnpacker& rhs);
  int operator==(const CLLNLUnpacker& rhs) const;
  int operator!=(const CLLNLUnpacker& rhs) const;

  // Virtual overrides:
  
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder);

  // Utilities:

private:
  void fetchSuperEvent(TranslatorPointer<UShort_t>& p);
  void unpackModule( CEvent& rEvent);
  uint16_t peekw();
  uint16_t getw();
  uint32_t getl();
  uint32_t getGoodl();
  
};

#endif
