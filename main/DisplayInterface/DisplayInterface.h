#ifndef DISPLAYINTERFACE_H
#define DISPLAYINTERFACE_H

#include "DisplayCollection.h"
#include "DisplayFactory.h"

#include <map>
#include <string>

class CDisplay;

/*!
 * \brief Class that controls interaction with display system
 *
 *  The collection of displays that are in use, need to be owned by
 *  something and this class does that. It owns the actual
 */
class CDisplayInterface
{
public:
    enum Result { DISPLAY_EXISTS, NO_CREATOR, SUCCESS };

private:
    std::map<std::string, CDisplay*> m_displays;

    CDisplayCollection    m_collection;
    CDisplayFactory       m_factory;

public:
    CDisplayInterface();
    virtual ~CDisplayInterface();

    /*!
     * \brief Create a new display
     *
     * \param name    - name to associate with display
     * \param type    - type of display (creator must exist with this type)
     * \return Status
     */
    Result createDisplay(const std::string& name, const std::string& type);

    CDisplay* getDisplay(const std::string& name);

    CDisplay* getCurrentDisplay();
    bool      setCurrentDisplay(const std::string& name);

    const CDisplayCollection& getCollection() const { return m_collection;}

    CDisplayFactory& getFactory() { return m_factory;}

};

#endif // DISPLAYINTERFACE_H
