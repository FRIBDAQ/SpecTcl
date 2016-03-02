#include "SpecTclInterfaceControl.h"

#include "SpecTclInterface.h"

namespace Viewer {

SpecTclInterfaceControl::SpecTclInterfaceControl(std::shared_ptr<SpecTclInterface> pInterface)
    : m_pInterface(pInterface),
      m_interfaceObservers()
{
}

void SpecTclInterfaceControl::setInterface(std::shared_ptr<SpecTclInterface> pInterface)
{
    bool gatePollingEnabled = m_pInterface->gatePollingEnabled();
    bool histPollingEnabled = m_pInterface->histogramInfoPollingEnabled();

    m_pInterface = pInterface;

    m_pInterface->enableGatePolling(gatePollingEnabled);
    m_pInterface->enableHistogramInfoPolling(histPollingEnabled);

    notifyObservers();
}

void SpecTclInterfaceControl::addSpecTclInterfaceObserver(std::unique_ptr<SpecTclInterfaceObserver> pObserver)
{
    m_interfaceObservers.insert(m_interfaceObservers.end(), std::move(pObserver));
}

void SpecTclInterfaceControl::notifyObservers()
{
    for(auto& pObserver : m_interfaceObservers) {
        pObserver->update(m_pInterface);
    }
}

} // namespace Viewer
