//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#include "DisplayCollection.h"
#include "Display.h"
#include <stdexcept>

using namespace std;

CDisplayCollection::CDisplayCollection()
    : m_displays(),
      m_current(m_displays.end())
{
}

bool CDisplayCollection::displayExists(CDisplay *pDisplay)
{
    Container::iterator found = m_displays.find(pDisplay);
    return (found != m_displays.end());
}

void CDisplayCollection::addDisplay(CDisplay* pDisplay)
{

    m_displays.insert(pDisplay);

//    if (result.second == true) {
//        if (m_displays.size() == 1) {
//            // there was no display before this, we make the only display
//            // the current focus
//            m_current = result.first;
//        }
//    }
}


void CDisplayCollection::removeDisplay(CDisplay *pDisplay)
{
    Container::iterator found = m_displays.find(pDisplay);
    if (found != m_displays.end()) {
        if (found == m_current) {
            m_current = m_displays.end();
        }
        m_displays.erase(found);
    } // else the display does not exist... we needn't remove it.

}


CDisplay* CDisplayCollection::getCurrentDisplay()
{
    if (m_current != m_displays.end()) {
        return *m_current;
    } else {
        return NULL;
    }
}


void CDisplayCollection::setCurrentDisplay(CDisplay* pDisplay)
{
    if (pDisplay == NULL) {
        throw runtime_error("CDisplayCollection::setCurrentDisplay(CDisplay*) passed null ptr.");
    }

    Container::iterator found = m_displays.find(pDisplay);
    if (found == m_displays.end()) {
        addDisplay(pDisplay);
        setCurrentDisplay(pDisplay);
    } else {
        m_current = found;
    }
}

////////////////////////////////////////////////////////////////////////
