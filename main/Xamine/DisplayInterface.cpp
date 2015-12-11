/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2015.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Jeromy Tompkins
         NSCL
         Michigan State University
         East Lansing, MI 48824-1321
*/

#include <DisplayInterface.h>

#include <string>
#include <assert.h>
#include <sstream>

using namespace std;

// Static member data:


CDisplayInterface::CDisplayInterface() {}

CDisplayInterface::CDisplayInterface(const CDisplayInterface&) {}

CDisplayInterface::~CDisplayInterface() {}



CDisplay *CDisplayInterface::getDisplay() const
{
    return m_pDisplay;
}

void CDisplayInterface::setDisplay(CDisplay *pDisplay)
{
    m_pDisplay = pDisplay;
}


