/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

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
#include "handleManager.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/**
 ** Construction - simply save the prefix and initialize the index to 0.
 ** @param prefix - handle prefix.  handle names will be of the form prefix_number
 */
CHandleManager::CHandleManager(string prefix) :
  m_prefix(prefix),
  m_index(0)
{}

/**
 ** Allocate a new handle and save the passed in pointer under that name.
 ** @param p  - Pointer for which to create a handle.
 ** @return std::string
 ** @retval Name allocated to this handle.
 **
 */
string
CHandleManager::add(void* p)
{

  // Construct the handle:


  size_t nameSize = m_prefix.size() + 100;
  char* pName     = new char[nameSize];
  sprintf(pName, "%s_%d", m_prefix.c_str(), m_index);
  string handle(pName);
  delete []pName;
  m_index++;

  // Insert the entry;

  m_handles[handle] = p;

  return handle;
}
/**
 ** Locate the handle associated with a name.  The assumption is that the handle
 ** pool will not contain null pointers.
 ** @param handle - Handle to find.
 ** @return void*
 ** @retval NULL - not found
 ** @retval other- Handle retrieved from the pool.
 */
void*
CHandleManager::find(string handle)
{
  HandleIterator p = m_handles.find(handle);
  if (p == m_handles.end()) {
    return NULL;
  }

  return p->second;
}
/**
 ** unregister removes a handle from the handle pool.
 ** If the handle does not exist this is a noop.
 ** @param handle - the handle to remove.
 */
void
CHandleManager::unregister(string handle)
{
  HandleIterator p = m_handles.find(handle);
  if(p != m_handles.end()) {
    m_handles.erase(p);
  }
}
