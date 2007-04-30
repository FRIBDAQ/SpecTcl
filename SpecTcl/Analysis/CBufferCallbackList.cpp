/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/

// Implementation of the CBufferCallbackList class.  See the header for
// more usage information.

#include <config.h>
#include "CBufferCallbackList.h"
#include <assert.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

#include <Iostream.h>

/*!
   Construct a callback list.  The STL list will construct itself just fine,
   we only need to save the buffer type.
   \param nBufferType (unsigned int [in]):
       The buffer type we will supposedly handle.
*/
CBufferCallbackList::CBufferCallbackList(unsigned int nBufferType) : 
  m_nBufferType(nBufferType)
{}

/*!
  Copy construction:   do a deep copy of the callbacks in the list and a
  simple copy of the data types.  Note that copy construction may be cause
  for some client level complication in reasoning about the required lifetimes
  of the callbacks themselves, as we are only keeping pointers otherwise
  the callback types will get sliced.

*/
CBufferCallbackList::CBufferCallbackList(const CBufferCallbackList& rhs) :
  m_nBufferType(rhs.m_nBufferType),
  m_Callbacks(rhs.m_Callbacks)
{}
/*!
   Destruction is a no=op because STL lists take care of their own
   destruction
*/
CBufferCallbackList::~CBufferCallbackList()
{
}

/*!
  Assignment is a deep assignment of the list contents and a copy 
  of the buffer type.
*/
CBufferCallbackList&
CBufferCallbackList::operator=(const CBufferCallbackList& rhs)
{
  if(this != &rhs) {
    m_nBufferType = rhs.m_nBufferType;
    m_Callbacks   = rhs.m_Callbacks; // This is a deep copy.
  }
  return *this;
}
/*!
   Equality comparison requires the same buffer type and list contents.
*/
int
CBufferCallbackList::operator==(const CBufferCallbackList& rhs) const
{
  return ((m_nBufferType    == rhs.m_nBufferType)      &&
	  (m_Callbacks      == rhs.m_Callbacks));
}
/*!
   Inequality means 'not equal':
*/
int
CBufferCallbackList::operator!=(const CBufferCallbackList& rhs) const
{
  return !(*this == rhs);
}
/*!
   Add a callback to the end of the  list of callbacks.
   \param callback (CBufferCallback&):
       A reference to the callback to add.  Note that a pointer to this
       callback is added, hence the need to pass by reference.
*/
void
CBufferCallbackList::addCallback(CBufferCallback& callback)
{
  m_Callbacks.push_back(&callback);
}
/*!
   Remove a callback by iterator from the list.  The iterator will usually
   be gotten by iterating the list of callbacks looking for the item
   that matches the one we want to delete.  See also begin() and end()
   \param itemPointer (CallbackIterator&):
      Reference to the item in the callback list.
  
   \note  The itemPointer and all other currently active iterators of iteration
   will be invalidated by this operation.
*/
void 
CBufferCallbackList::removeCallback(CallbackIterator& itemPointer)
{
  m_Callbacks.erase(itemPointer);
}
/*!
   Return a begin of iteration iterator.  This is usually used in 
   code that looks like:

   \verbatim
   CBufferCallbackList cblist;
    ...
   CBufferCallbackList::CallbackIterator p = cblist.begin();
   while(p != cblist.end()) {
      // Do something with *p
      ...
      p++;
   }
   // At this time all elements were visited unless a break was done in the loop.
   \endverbatim
*/
CBufferCallbackList::CallbackIterator
CBufferCallbackList::begin()
{
  return  m_Callbacks.begin();
}
/*!
   Return the end of iteration iterator.  See begin() for more information about
   how this is typically used.
*/
CBufferCallbackList::CallbackIterator
CBufferCallbackList::end()
{
  return m_Callbacks.end();
}

/*!
   Return the number of callbacks that have been registered on this list:
*/
int
CBufferCallbackList::size() const
{
  return m_Callbacks.size();
}
/*!
   Visit each callback, calling them for the particular buffer we have.
   \param nBufferType (unsigned int [in]):
      we validate that this buffer type matches the one we have saved, via
      an assert().
   \param pBuffer (const void [in]):
      Pointer to the buffer that is initiating the callback.
*/
void
CBufferCallbackList::operator()(unsigned int nBufferType, const void* pBuffer)
{
  assert(nBufferType == m_nBufferType);


  CallbackIterator i = begin();
  while(i != end()) {
    (**i)(nBufferType, pBuffer);

    i++;
  }
}
