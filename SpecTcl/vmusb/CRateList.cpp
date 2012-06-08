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
#include "CRateList.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


CRateList* CRateList::m_pInstance(0);

/*!
   Constructor is no-op but must be implemented so that it can be privatized.
*/

CRateList::CRateList() {}

/*!  Return the singleton instance of the class.
 */
CRateList*
CRateList::getInstance()
{
  if (!m_pInstance) {
    m_pInstance = new CRateList;
  }
  return m_pInstance;
}




////////////////////////////////////////////////////////////////////////
//////////////////// List operations ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/*
  Add a processor to the tail of the list.
*/
void 
CRateList::addProcessor(CRateProcessor& processor)
{
  CRateProcessor* p = &processor;

  Entry e(p, false);
  m_rates.push_back(e);
}
/*!
  Remove a processor from the list... first we need to find it.
  - no-op if it does not exist.
*/
void
CRateList::deleteProcessor(CRateProcessor& processor)
{
  RateListIterator  i = find(processor);
  if (i != end()) {
    m_rates.erase(i);
  }

}

/*!
   Mark an item for deletion.
*/
void
CRateList::markForDeletion(CRateProcessor& processor)
{
  RateListIterator i= find(processor);
  if (i != end()) {
    i->second = true;
  }

}
////////////////////////////////////////////////////////////////////////
//////////////////////////////// List iteration ////////////////////////
////////////////////////////////////////////////////////////////////////

/*!
   Find an event processor.  Returns end() if failure.

  
*/
CRateList::RateListIterator
CRateList::find(CRateProcessor& processor)
{
  RateListIterator i = begin();
  while (i != end()) {
    if (i->first == &processor) break;
    i++;
  }

  return i;
}

CRateList::RateListIterator
CRateList::begin() {
  return m_rates.begin();
}

CRateList::RateListIterator
CRateList::end() {
  return m_rates.end();
}
