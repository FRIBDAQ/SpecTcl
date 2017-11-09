/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

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

/** @file:  CEventLeaf.cpp
 *  @brief: Implement Root leaf that's built on SpecTcl CEvent object.
 */
#include "CEventLeaf.h"
#include <Event.h>
#include <cmath>
#include <TBuffer.h>


/** static member data: */

CEvent* CEventLeaf::m_pCurrentEvent(0);

CEventLeaf::CEventLeaf() {}
/**
 *  Constructor
 *    @param parent - branch this leaf lives in.
 *    @param name   - name of the leaf (tail of the variable name).
 *    @param id     - Element of the rEvent 'vector' that supplies this parameter.
 */
CEventLeaf::CEventLeaf(TBranch* parent, const char* name, unsigned id) :
    TLeafD(parent, name, "D"),
    m_nId(id)
{}

/**
 * GetValue
 *    If there is a value for the parameter in the m_pCurrentEvent vectork
 *    it is returned, if not a quiet NaN is returned instead.
 *  @return Double_t
 */
Double_t
CEventLeaf::GetValue(Int_t i) const
{
    if (m_pCurrentEvent->size() < m_nId) {
        if ((*m_pCurrentEvent)[m_nId].isValid()) {
            return (*m_pCurrentEvent)[m_nId];
        }
    }
    return std::nan("1");
}

void
CEventLeaf::FillBasket(TBuffer& b)
{
    Int_t len = sizeof(Double_t);
    Double_t value = GetValue(0);
    b.WriteFastArray(&value, len);
}
/**
 * setEvent - static method to set the current event
 *
 *  @param r - refers to the current event.
 */
void
CEventLeaf::setEvent(CEvent& r)
{
    m_pCurrentEvent = &r;
}
ClassImp(CEventLeaf)