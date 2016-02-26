#include "SpecTclShMemInterface.h"
#include "SpecTclRESTInterface.h"

namespace Viewer {

SpecTclShMemInterface::SpecTclShMemInterface() :
    SpecTclInterface(),
    m_pRESTInterface(new SpecTclRESTInterface)
{
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

GateList* SpecTclShMemInterface::getGateList()
{
    m_pRESTInterface->getGateList();
}

void SpecTclShMemInterface::enableHistogramInfoPolling(bool enable)
{
    m_pRESTInterface->enableHistogramInfoPolling(enable);
}

HistogramList* SpecTclShMemInterface::getHistogramList()
{
    m_pRESTInterface->getHistogramList();
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

 } // namespace VIewer
