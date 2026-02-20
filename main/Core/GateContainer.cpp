/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


static const char* Copyright =
  "(C) Copyright Michigan State University 2007, All rights reserved";

// GateContainer.cpp
//   This class allows Gates to be treated as 
//   named items which have constant 'pointers'
//   even if the gate itslef changes.  This is provided
//   for the convenience of CCompoundGate objects.
//   By retaining CGateContainer object references and not
//   CGate object reference, underlying gates can be 
//   modified etc.

//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//
#include <config.h>
#include "GateContainer.h"                               

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Functions for class CGateContainer

////////////////////////////////////////////////////////////////////////////
//
// Function:
//    CGateContainer (std::string& sName, UInt_t nNumber, 
//		      const CGate& rGate)
// Operation Type:
//    Constructor.
//
CGateContainer::CGateContainer(const std::string& sName, UInt_t nNumber, 
			       CGate& rGate) :
  CNamedItem(sName, nNumber),
  m_pGate(rGate.clone())
{
}

///////////////////////////////////////////////////////////////////////////
//
// Function:
//    ~CGateContainer()
// Operation Type:
//     Destructor.
//
CGateContainer::~CGateContainer() {
  delete m_pGate;
}

/////////////////////////////////////////////////////////////////////////
//
// Function:
//   CGateContainer(const CGateContainer& rhs)
// Operation Type:
//   Copy construction
//
CGateContainer::CGateContainer(const CGateContainer& rhs) :
  CNamedItem(rhs),
  m_pGate(0)
{
  if(rhs.m_pGate) m_pGate = rhs.m_pGate->clone();
}

/////////////////////////////////////////////////////////////////////////
//
//  Function:
//     operator=(const CGateContainer& aCGateContainer)
//  Operation Type:
//     Assignment.
//
CGateContainer&
CGateContainer::operator=(const CGateContainer& aCGateContainer) {
  if(this != &aCGateContainer) {
    CNamedItem::operator=(aCGateContainer);
    delete m_pGate;
    m_pGate = 0;
    if(aCGateContainer.m_pGate)
      m_pGate = aCGateContainer.m_pGate->clone();
  }
  return *this;
}
// metadata wrapper methods (issue #229).

/**
 *  set a metadata item.
 * @param name - name of the item.
 * @param value - value of the metadata.
 * @note if the item does not exist it is created otherwise it's value is overwritten.
 */
void 
CGateContainer::setMetadata(const char* name, const char* value) {
  m_metadata.set(name, value);
}
/**
 * return the value of a metadata item.
 * 
 * @param name - name of the item.
 * @return std::string - value of the item.
 * @throw CNoSuchObjectException - if the item does not exist.
 */
std::string
CGateContainer::getMetadata(const char* name) {
  return m_metadata.get(name);
}
/**
 * Return a const reference to the metadata container.  This can
 * be treated as an std::map<std::string, std::string> where
 * the keys are  item names and the values the values of the item.
 * 
 * @return const CMetadata::Metadata_t&
 */
const CMetadata::Metadata_t&
CGateContainer::getAllMetadata() const {
  return m_metadata.get_all();
}