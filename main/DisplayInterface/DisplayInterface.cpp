#include "DisplayInterface.h"

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

