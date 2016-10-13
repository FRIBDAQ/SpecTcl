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

#include "DisplayInterface.h"
#include "Display.h"

using namespace std;

CDisplayInterface::CDisplayInterface()
    : m_displays(),
      m_collection(),
      m_factory()
{
}

CDisplayInterface::~CDisplayInterface()
{
    typedef map<string, CDisplay*>::iterator iterator;
    iterator it = m_displays.begin();
    iterator end   = m_displays.end();

    while (it != end) {
        delete it->second;
        ++it;
    }
}

CDisplayInterface::Result
CDisplayInterface::createDisplay(const std::string &name, const std::string &type)
{
    typedef map<string, CDisplay*>::iterator iterator;

    Result retVal = SUCCESS;
    iterator found = m_displays.find(name);
    if (found != m_displays.end()) {
        // this has already been created.
        retVal = DISPLAY_EXISTS;
    } else {
        CDisplay* pDisp = m_factory.create(type);
        if (pDisp) {
            m_displays[name] = pDisp;
            m_collection.addDisplay(pDisp);
            retVal = SUCCESS;
            if (m_displays.size() == 1) {
                setCurrentDisplay(name);
            }
        } else {
            retVal = NO_CREATOR;
        }
    }

    return retVal;
}

CDisplay* CDisplayInterface::getDisplay(const string &name)
{
    CDisplay* pDisplay = NULL;

    typedef map<string, CDisplay*>::iterator iterator;

    iterator found = m_displays.find(name);
    if (found != m_displays.end()) {
        return found->second;
    }

   return pDisplay;
}

CDisplay* CDisplayInterface::getCurrentDisplay()
{
    return m_collection.getCurrentDisplay();
}


bool CDisplayInterface::setCurrentDisplay(const string &name)
{
    bool success = false;

    typedef map<string, CDisplay*>::iterator iterator;

    iterator found = m_displays.find(name);
    if (found != m_displays.end()) {
        m_collection.setCurrentDisplay(found->second);
        success = true;
    }

    return success;
}

