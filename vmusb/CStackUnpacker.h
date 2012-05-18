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

#ifndef __CSTACKUNPACKER_H
#define __CSTACKUNPACKER_H


#ifndef __EVENTPROCESSOR_H
#include <EventProcessor.h>
#endif

#ifndef __TRANSLATORPOINTER_H
#include <TranslatorPointer.h>
#endif


#ifndef __CRT_STDINT
#include <stdint.h>
#ifndef __CRT_STDINT
#define __CRT_STDINT
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

class CModuleUnpacker;
class CEvent;
class CAnalyzer;
class CBufferDecoder;

/*!
  This class unpacks data from the generalized stack VM-USB readout program.
  Events have format that is described in the VM-USB manual section 4.6.
  
  Events come in fragments that are at most 2kwords long.  A fragment has 
  a header that looks like:

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

  In this software we will assemble events into a vector decoding the stack id
  as we go.   The stack id will be used to fetch a vector of pointers to 
  parameter maps for each of the modules.  Each parameter map also includes
  information that allows the unpacker to choose a correct module unpacker for
  the map.  The actual unpacking of the data from a piece of hardware is delegated to 
  the appropriate module specific unpacker.
*/

class CStackUnpacker  : public CEventProcessor
{
private:
  // data structures:
  typedef struct _StackInfo {
    int         s_stackNumber;
    size_t      s_stackSize;
  } StackInfo;

  // static data:

  static CModuleUnpacker* m_unpackers[];

  // Canonicals:
public:
  CStackUnpacker();
  virtual ~CStackUnpacker();

  // As we have no object data the default canonicals are just fine for us.

  // Virtual overrides.

public:
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder);

  // Utilties:

private:
  static StackInfo assembleEvent(TranslatorPointer<UShort_t>& p, std::vector<uint16_t>& event);
};

#endif
