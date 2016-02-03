#ifndef DISPLAYCOLLECTION_H
#define DISPLAYCOLLECTION_H

#include <set>
#include <stdlib.h>

class CDisplay;

/*!
 * \brief The manager class for display
 *
 * The DisplayManager class is entirely responsible for maintaining
 * the list of displays that SpecTcl knows about. It does not own
 * any of the display instances that are registered to it. Instead it
 * is expected that some other entity will own the display and this will
 * just provide a list of viable displays that can be set to current.
 */
class CDisplayCollection
{

private:
    typedef std::set<CDisplay*>            Container;
    typedef typename Container::iterator   Iterator;
    typedef typename Container::value_type ValueType;

    Container                m_displays;
    Container::iterator      m_current;

public:
    CDisplayCollection();

    bool displayExists(CDisplay* pDisplay);

    void addDisplay(CDisplay* pDisplay);
    void removeDisplay(CDisplay* pDisplay);

    CDisplay* getCurrentDisplay();
    void      setCurrentDisplay(CDisplay* pDisplay);

    Iterator begin() { return m_displays.begin(); }
    Iterator end()   { return m_displays.end(); }

    size_t size() const { return m_displays.size(); }
};


#endif // DISPLAYCOLLECTION_H
