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
#include "DopeVector.h"
#include <RangeError.h>

/////////////////////////////////////////////////////////////////////////////
//
// Implementation of canonicals:

/*!
   Construction.. we just need to start the dope vector out with no elements:
*/
DopeVector::DopeVector()
{
  reset();
}
/*!
  Copy construction, need to copy both the size and vector.
  @param rhs - Dope vector we are creating a clone of
*/
DopeVector::DopeVector(const DopeVector& rhs) : 
  m_dopeSize(rhs.m_dopeSize),
  m_indices(rhs.m_indices)

{
}
/*!
  Destruction .. no op
*/
DopeVector::~DopeVector()
{
}

/*!
  Assignment:
  @param rhs reference to the the guy we are copying into this.
  @return DopeVector&
  @retval *this
*/
DopeVector&
DopeVector::operator=(const DopeVector& rhs)
{
  if (this != &rhs) {
    m_dopeSize = rhs.m_dopeSize;
    m_indices  = rhs.m_indices;
  }
  return *this;
}

/////////////////////////////////////////////////////////////////////////////
//
//  Implementation of object operations.

/*!
   Reset the dope vector to empty.  In order to avoid continually resizeing
   the vector, we just reset the m_dopeSize value:

*/
void
DopeVector::reset()
{
  m_dopeSize = 0;
}
/*!
   Add a new element to the end of the dope vector.
   @param index - new index to add.
   @note if necessary the vector will be expanded to hold the new element.
*/
void
DopeVector::append(unsigned int index)
{
  if (m_dopeSize < m_indices.size()) {
    m_indices[m_dopeSize] = index;
  }
  else {
    m_indices.push_back(index);
  }
  m_dopeSize++;

}
/*!
   Return the number of elements in the dope vector.
   This is m_dopeSize rather than m_indices.size()
   @return unsigned int
   @retval m_dopeSize
*/
unsigned int
DopeVector::size() const
{
  return m_dopeSize;
}
/*!
  Indexing operator.
  @param index -- which element to grab.
  @return unsigned int&
  @retval reference to the indexed element of the dope vector.
  @throws CRangeError if the indwex is out of range
*/
unsigned int&
DopeVector::operator[](int index)
{
  if ((index < 0) || (index >= m_dopeSize)) {
    throw CRangeError(0, m_dopeSize-1, index,
		      "Indexing in a Dope Vector");
  }
  return m_indices[index];
}
