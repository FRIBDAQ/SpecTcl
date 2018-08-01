#ifndef DYNAMICLIST_H
#define DYNAMICLIST_H
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

/*!
   DynamicList is a template class that maintains a dynamically
   extensible array of type T*.  The array is terminated by a
   NULL pointer.  It extends itself as needed with a settable
   granularity.

*/
template <class T>
class DynamicList 
{
private:
  unsigned int m_elements;	// Number of elements currently in the list.
  unsigned int m_maxElements;	// Number this list will hold before needing expansion
  unsigned int m_extendSize;    // How many elements are added to the list on expansion.
  T**          m_pTheElements;  // The list itself.
public:
  DynamicList();
  ~DynamicList();

  // Other canonicals:

public:
  DynamicList(const DynamicList& rhs);
  DynamicList& operator=(const DynamicList& rhs);

  // Comparisons don't make sense so:

private:
  int operator==(const DynamicList& rhs) const;
  int operator!=(const DynamicList& rhs) const;



  // Member functions that actually do something:
  
public:

  void add(T* element);
  void remove(T* element);
  T**  getList();
  void setExtendSize(unsigned int count);
  unsigned int extendSize() const;

  unsigned int capacity() const {
    return m_maxElements;
  }

private:
  void clone_in(const DynamicList& rhs);

};
#include "DynamicList.cpp"

#endif
