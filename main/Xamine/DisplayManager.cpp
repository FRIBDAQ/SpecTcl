#include "DisplayManager.h"
#include "Display.h"

using namespace std;

CDisplayManager::CDisplayManager()
{
}

bool CDisplayManager::addDisplay(CDisplay* pDisplay)
{

    pair<Container::iterator, bool> result = m_displays.insert(pDisplay);

    if (result.second == true) {
        if (m_displays.size() == 1) {
            // there was no display before this, we make the only display
            // the current focus
            m_current = result.first;
        }
    }

    return result.second;
}

void CDisplayManager::removeDisplay(CDisplay *pDisplay)
{
    Container::iterator found = m_displays.find(pDisplay);
    if (found != m_displays.end()) {
        if (found == m_current) {
            m_current = m_displays.end();
        }
        m_displays.erase(found);
    } // else the display does not exist... we needn't remove it.

}

CDisplay* CDisplayManager::getCurrentDisplay() const
{
    if (m_current != m_displays.end()) {
        return *m_current;
    } else {
        return NULL;
    }
}

void CDisplayManager::setCurrentDisplay(CDisplay* pDisplay)
{
    Container::iterator found = m_displays.find(pDisplay);
    if (found == m_displays.end()) {
        addDisplay(pDisplay);
        setCurrentDisplay(pDisplay);
    } else {
        m_current = found;
    }
}

////////////////////////////////////////////////////////////////////////
