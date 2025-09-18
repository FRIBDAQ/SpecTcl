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

#ifndef CSTACKUNPACKERMVLC_H
#define CSTACKUNPACKERMVLC_H

#include <EventProcessor.h>
#include <TranslatorPointer.h>
#include <stdint.h>
#include <vector>

class CModuleUnpacker;
class CEvent;
class CAnalyzer;
class CBufferDecoder;

/*!
  This class unpacks stacks from fribdaq-readout in the
  mesytec-mvlc installation.  These stacks are
  very  much like VMUSB stacks _but_ and this is a big but.
  *  They are not segmented.  Events are all a single soup.
  *  They don't have a header.
  

  There are other differences that the individaul decoders
  need to know as well but we don't worry about that here.
*/

class CStackUnpackerMVLC  : public CEventProcessor
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
  CStackUnpackerMVLC();
  virtual ~CStackUnpackerMVLC();

  // As we have no object data the default canonicals are just fine for us.

  // Virtual overrides.

public:
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder);

  // Utilties:

private:
  static StackInfo assembleEvent(size_t nBytes, TranslatorPointer<UShort_t>& p, std::vector<uint16_t>& event);
};

#endif
