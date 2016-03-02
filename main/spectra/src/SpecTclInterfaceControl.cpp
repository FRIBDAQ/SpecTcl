#include "SpecTclInterfaceControl.h"

namespace Viewer {

SpecTclInterfaceControl::SpecTclInterfaceControl(std::shared_ptr<SpecTclInterface> pInterface)
    : m_pInterface(pInterface),
      m_interfaceObservers()
{
}

void SpecTclInterfaceControl::setSpecTclInterface(std::unique_ptr<SpecTclInterface> pInterface)
{

    m_pInterface = std::shared_ptr<SpecTclInterface>( std::move(pInterface) );

    notifyObservers();
}

void SpecTclInterfaceControl::addSpecTclInterfaceObserver(std::unique_ptr<SpecTclInterfaceObserver> pObserver)
{
    m_interfaceObservers.insert(m_interfaceObservers.end(), std::move(pObserver));
}

void SpecTclInterfaceControl::notifyObservers()
{
    for(auto& pObserver : m_interfaceObservers) {
        pObserver->update(m_pSpecTcl);
    }
}

} // namespace Viewer
