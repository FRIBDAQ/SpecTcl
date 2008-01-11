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
#include "CFlattenedGateList.h"

/*!
  Default constructor:
  The GateList constructor takes care of itself.
*/
CFlattenedGateList::CFlattenedGateList()
{
}
/*!
  Destructor... again the gate list destructor takes care of everything:

*/
CFlattenedGateList::~CFlattenedGateList()
{}

/*!
  Copy consruction.. just need to copy construct the gate list:

*/
CFlattenedGateList::CFlattenedGateList(const CFlattenedGateList& rhs) :
  m_Gates(rhs.m_Gates)
{
}
/*
  Assignment.. just need to assign the gate list:
*/
CFlattenedGateList&
CFlattenedGateList::operator=(const CFlattenedGateList& rhs)
{
  m_Gates = rhs.m_Gates;

  return *this;
}

/*!
  Return the contents of the gate list.. this is delegated to m_Gates.
*/
CGateContainer**
CFlattenedGateList::getList()
{
  return m_Gates.getList();
}

/*!
  Clear the cache for all gates in the list.  This is a matter of 
  iterating through the list invoking Reset on all gates.
*/
void
CFlattenedGateList::clearCache()
{
  CGateContainer** theList = m_Gates.getList();
  
  if(theList) {
    while (*theList) {
      (**theList)->Reset();
      theList++;
    }
  }
}


/*!
   Reacts to the addition of  new gate to the gate dictionary.
This is only called for gates that don't yet exist.  Insert the
gate at the end of the flattened gates list.

\param name       - Name of the new gate.
\param container  - The container that fronts the gate.

*/
void 
CFlattenedGateList::onAdd(std::string name, CGateContainer& item)
{
  if (item->caches()) {
    m_Gates.add(&item);
  }

}
/*!
  Unconditionally remove the specified gate.  We don't bother to check
if the gate is actually in the list in the first place (is a caching gate),
since the remove function is a no-op for items not in the list.

Since SpecTcl can swap around gate containers, we need
to match by name, not by pointer and then remove the right one:

*/
void 
CFlattenedGateList::onRemove(std::string name, CGateContainer& item)
{
  CGateContainer** pGates = getList();
  if (pGates) {
    while (*pGates) {
      CGateContainer* candidate = *pGates;
      if (name == candidate->getName()) {
	m_Gates.remove(candidate);
	return;
      }
      pGates++;
    }
  }
}

/*
  Simplest way to handle changes in gates are to delete and then
  add ... add checks for cached-ness.

*/
void 
CFlattenedGateList::onChange(std::string name, CGateContainer& item)
{
  onRemove(name, item);
  onAdd(name, item);
}




