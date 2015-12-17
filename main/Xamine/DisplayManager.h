#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <set>

class CDisplay;


class CDisplayManager
{

private:
    typedef std::set<CDisplay*>  Container;
    typedef typename Container::value_type ValueType;

    Container                m_displays;
    Container::iterator      m_current;

public:
    CDisplayManager();

    bool addDisplay(CDisplay* pDisplay);
    void removeDisplay(CDisplay* pDisplay);

    CDisplay* getCurrentDisplay() const;
    void setCurrentDisplay(CDisplay* pDisplay);
};


#endif // DISPLAYMANAGER_H
