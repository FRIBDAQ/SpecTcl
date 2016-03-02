#ifndef VIEWER_SPECTCLINTERFACECONTROL_H
#define VIEWER_SPECTCLINTERFACECONTROL_H

#include "SpecTclInterfaceObserver.h"

#include <list>
#include <memory>

namespace Viewer {

class SpecTclInterface;

/*!
 * \brief The SpecTclInterfaceControl class
 *
 *  Controls the unique instance of the SpecTclInterface for the program.
 *  The SpecTclInterface is not a singleton becuase the program supports
 *  switching between local and remote.
 */
class SpecTclInterfaceControl
{
public:

    SpecTclInterfaceControl(std::shared_ptr<SpecTclInterface> pInterface);

    std::shared_ptr<SpecTclInterface> getInterface() const {
        return m_pInterface;
    }

    void setInterface(std::shared_ptr<SpecTclInterface> pInterface);

    void addSpecTclInterfaceObserver(std::unique_ptr<SpecTclInterfaceObserver> pObserver);

    template<class T> void addGenericSpecTclInterfaceObserver(T& obj);

    void notifyObservers();

private:
    std::shared_ptr<SpecTclInterface>                     m_pInterface;
    std::list<std::unique_ptr<SpecTclInterfaceObserver> > m_interfaceObservers;

};


// implementation of template functions
template<class T>
void SpecTclInterfaceControl::addGenericSpecTclInterfaceObserver(T& obj) {

    using GenericObserver = GenericSpecTclInterfaceObserver<T>;

    std::unique_ptr<SpecTclInterfaceObserver> pObserver(new GenericObserver(obj));

    addSpecTclInterfaceObserver(std::move(pObserver));

}

} // namespace Viewer

#endif // VIEWER_SPECTCLINTERFACECONTROL_H
