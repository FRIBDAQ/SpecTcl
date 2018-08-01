/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/

#ifndef CBUFFERCALLBACKLIST_H
#define CBUFFERCALLBACKLIST_H

#include "CBufferCallback.h"
#include <list>

/*!
   This class encapsulates an ordered list of callbacks.
   The idea is that the buffer processor will have a set of these,
   one for each buffer type on which at least one callback has been established.
   This class factors out the infrastructure for maintaining the callback lists
   as well as traversing them when the callback is fired.

   Note that this class does not assume responsibility for the 'ownership' of 
   callback objects.  The creator of those objects is responsible for maintaining
   their scope and lifetime.

*/
class CBufferCallbackList
{
  // Exported data types;
public:
  typedef std::list<CBufferCallback*> CallbackList;
  typedef CallbackList::iterator CallbackIterator;

  // Class data:
private:
  unsigned int  m_nBufferType;
  CallbackList  m_Callbacks;

public:
  CBufferCallbackList(unsigned int nBufferType);
  CBufferCallbackList(const CBufferCallbackList& rhs);
  virtual ~CBufferCallbackList();

  CBufferCallbackList& operator=(const CBufferCallbackList& rhs);
  int operator==(const CBufferCallbackList& rhs) const;
  int operator!=(const CBufferCallbackList& rhs) const;

  // Class memmber functions:

  void addCallback(CBufferCallback& callback);
  void removeCallback(CallbackIterator& itemPointer);
  CallbackIterator begin();
  CallbackIterator end();
  int size() const;

  virtual void operator()(unsigned int nBufferType, const void* pBuffer);

};



#endif
