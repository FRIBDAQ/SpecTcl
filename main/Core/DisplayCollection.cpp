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
