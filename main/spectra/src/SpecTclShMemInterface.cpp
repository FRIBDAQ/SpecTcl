#include "SpecTclShMemInterface.h"
#include "SpecTclRESTInterface.h"

namespace Viewer {

SpecTclShMemInterface::SpecTclShMemInterface() :
    SpecTclInterface(),
    m_pRESTInterface(new SpecTclRESTInterface)
{
    connect(m_pRESTInterface.get(), SIGNAL(histogramContentUpdated(HistogramBundle*)),
            this, SLOT( onHistogramContentUpdated(HistogramBundle*)));

    connect(m_pRESTInterface.get(), SIGNAL(histogramListChanged()),
            this, SLOT(onHistogramListChanged()));

    connect(m_pRESTInterface.get(), SIGNAL(gateListChanged()),
            this, SLOT(onGateListChanged()));

}

SpecTclShMemInterface::~SpecTclShMemInterface()
{
}

void SpecTclShMemInterface::addGate(const GSlice &slice)
{
    m_pRESTInterface->addGate(slice);
}

void SpecTclShMemInterface::editGate(const GSlice &slice)
{
    m_pRESTInterface->editGate(slice);
}

void SpecTclShMemInterface::deleteGate(const GSlice &slice)
{
    m_pRESTInterface->deleteGate(slice);
}

void SpecTclShMemInterface::addGate(const GGate &gate)
{
    m_pRESTInterface->addGate(gate);
}

void SpecTclShMemInterface::editGate(const GGate &gate)
{
    m_pRESTInterface->editGate(gate);
}

void SpecTclShMemInterface::deleteGate(const GGate &gate)
{
    m_pRESTInterface->deleteGate(gate);
}

void SpecTclShMemInterface::deleteGate(const QString &name)
{
    m_pRESTInterface->deleteGate(name);
}

void SpecTclShMemInterface::enableGatePolling(bool enable)
{
    m_pRESTInterface->enableGatePolling(enable);
}

bool SpecTclShMemInterface::gatePollingEnabled() const {
    return m_pRESTInterface->gatePollingEnabled();
}

GateList* SpecTclShMemInterface::getGateList()
{
    return m_pRESTInterface->getGateList();
}

void SpecTclShMemInterface::enableHistogramInfoPolling(bool enable)
{
    m_pRESTInterface->enableHistogramInfoPolling(enable);
}

bool SpecTclShMemInterface::histogramInfoPollingEnabled() const
{
    return m_pRESTInterface->histogramInfoPollingEnabled();
}

HistogramList* SpecTclShMemInterface::getHistogramList()
{
    return m_pRESTInterface->getHistogramList();
}

void SpecTclShMemInterface::requestHistContentUpdate(QRootCanvas *pCanvas)
{

}

void SpecTclShMemInterface::requestHistContentUpdate(TPad *pPad)
{

}

void SpecTclShMemInterface::requestHistContentUpdate(const QString &hName)
{

}

void SpecTclShMemInterface::onHistogramContentUpdated(HistogramBundle *pBundle) {
    emit histogramContentUpdated(pBundle);
}

void SpecTclShMemInterface::onHistogramListChanged() {
    emit histogramListChanged();
}

void SpecTclShMemInterface::onGateListChanged() {
    emit gateListChanged();
}

} // namespace VIewer
