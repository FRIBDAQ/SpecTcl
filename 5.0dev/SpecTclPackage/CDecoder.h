/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef _CDECODER_H
#define _CDECODER_H

/**
 * @file CDecoder.h
 * @brief Defines the abstract base class for the event source decoder classes.
 */

#ifndef _STDC_STDINT_H
#include <stdint.h>
#ifndef _STDC_STDINT_H
#define _STDC_STDINT_H
#endif
#endif

#ifndef _STDC_STDLIB_H
#include <stdlib.h>
#ifndef _STDC_STDLIB_H
#define _STDC_STDLIB_H
#endif
#endif

class CDataSource;

/**
 * @class CDecoder
 *
 *   This is the abstract base class of all decoders.  Decoders are invoked by the 
 *   analysis pipeline and are supposed to know how to get individual items from the
 *   event source.  The analysis pipeline then interrogates information about the item
 *   which is, in turn passed down the event processing pipeline.
 *   
 *   The decoder is also modeled after a forward iterator, however the forward iterator operates
 *   on potentially a clump of items read from the decoder.  The next method advances to the
 *   next item in a clump and returns false if the clump is exhausted.  The
 *   onData method returns false.  The event pipeline determines when a data source itself
 *   is exhausted; exhausted data sources are considered readable, and prior to 
 *   starting this off, the pipeline must check the isEof method of the data source.  If the
 *   isEof method is true, the data source is exhausted and therefore closed.
 */

class CDecoder {
public:
  virtual ~CDecoder() {}	// So that class hierarchies will chain destructors.


  // This interface must be implemented by all concrete CDecoder classes.

  virtual void   onData(CDataSource* pSource)  = 0;
  virtual size_t getItemSize()                 = 0;
  virtual void*  getItemPointer()              = 0;
  virtual bool   next()                        = 0;
};


#endif
