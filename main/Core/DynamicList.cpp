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
#include "DynamicList.h"
#include <stdlib.h>


static const unsigned int DefaultExtendSize(256);

/*!
  Construct a dynamic list:
*/
template <class T>
DynamicList<T>::DynamicList() :
  m_elements(0),
  m_maxElements(0),
  m_extendSize(DefaultExtendSize),
  m_pTheElements(0)
{
}

/*!
  Destroy the list
*/
template<class T>
DynamicList<T>::~DynamicList()
{
  delete m_pTheElements;
}

/*!
  Copy construction we use clone_in to 
  copy the parameter into ourselves:

*/
template <class T>
DynamicList<T>::DynamicList(const DynamicList<T>& rhs) :
  m_elements(0),
  m_maxElements(0),
  m_extendSize(DefaultExtendSize),
  m_pTheElements(0)
{
  clone_in(rhs);
}

/*!
  Assignment is just like copy construction except that
  we are going to protect against self assignment and return
  *this when done:

*/
template <class T>
DynamicList<T>&
DynamicList<T>::operator=(const DynamicList<T>& rhs)
{
  if (&rhs != this) {
    clone_in(rhs);
  }
  return *this;
}

/*!
   Get the list pointer.
*/
template <class T>
T**
DynamicList<T>::getList()
{
  return m_pTheElements;
}

/*!
  Get the current extension size>
*/
template <class T>
unsigned int
DynamicList<T>::extendSize() const
{
  return m_extendSize;
}

/*!
  Add an element to the list.  If necessary, expanding
  the list. If m_extendSize is large, this is amortized constant time.

  \param element - the element to add.

*/
template <class T>
void
DynamicList<T>::add(T* element)
{
  if (!(m_pTheElements) || (m_elements >= m_maxElements-1)) { // -1 to ensure there's always a trailing null.


    if (m_pTheElements) {
      m_pTheElements = reinterpret_cast<T**>(realloc(m_pTheElements, 
						    (m_maxElements+m_extendSize)*sizeof(T*)));
      // Zero the new elements:

      for (int i = 0; i < m_extendSize; i++) {
	m_pTheElements[m_maxElements+i] = reinterpret_cast<T*>(0);
      }
    } 
    else {
      
      m_pTheElements = reinterpret_cast<T**>(calloc(m_extendSize, sizeof(T*)));// Null filled storage.

    }
    m_maxElements += m_extendSize;

  }
  m_pTheElements[m_elements] = element;
  m_elements++;
}

/*!
  Set a new size for the extend size.  The extend size determines the number of elements
  added to the list if it needs to be expanded.

  \param size  - The new extend size.
*/
template <class T>
void
DynamicList<T>::setExtendSize(unsigned int size)
{
  m_extendSize = size;
}
/*!
  Remove a specific element from the list.
  This is linear time.    The linear time requirement is to find the
  element.  There is no gaurantee of order preservation so we just
  put the last element in place of the deleted element and clear the
  former last element which is constant time once we find the element.

  \param element - Pointer to delete.

  \note it is a no-op to delete a nonexistent element.

*/
template <class T>
void
DynamicList<T>::remove(T* element)
{
  // This works for the case of no list as it won't get executed.

  for (int i =0; i < m_elements; i++) {
    if (m_pTheElements[i] == element) {
      m_elements--;
      m_pTheElements[i]  = m_pTheElements[m_elements]; 
      m_pTheElements[m_elements] = reinterpret_cast<T*>(0);
      return;
    }
  }
}

////  clone_in contains common code for the copy constructor and assignment
////  methods:

template <class T>
void
DynamicList<T>::clone_in(const DynamicList<T>& rhs)
{
  free( m_pTheElements);

  m_pTheElements = reinterpret_cast<T**>(calloc(rhs.m_maxElements, sizeof(T*)));
  memcpy(m_pTheElements, rhs.m_pTheElements, rhs.m_elements*sizeof(T));
  m_elements     = rhs.m_elements;
  m_maxElements  = rhs.m_maxElements;
  m_extendSize   = rhs.m_extendSize;

}
