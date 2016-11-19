/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/

#ifndef __CBUFFERCALLBACK_H
#define __CBUFFERCALLBACK_H

/*!
   The CBufferCallback is the abstract base class for buffer callbacks.
   buffer callbacks are function objects that are invoked when a buffer of
   a particular type is seen by the system.  Callbacks allow for a simple (for the
   user) scheme of associating code with buffer processing.

*/
class CBufferCallback
{
 public:

  // The constructor and destructor are just place holders for now.

  CBufferCallback() {}
  virtual ~CBufferCallback() {}

  int operator==(const CBufferCallback& rhs) const { return 1; }
  int operator!=(const CBufferCallback& rhs) const { return 0; }

  // Class functions:

  virtual void operator()(unsigned int nType, const void* pBuffer) = 0;

};


#endif
