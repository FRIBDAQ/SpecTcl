#include "SpecTclShMemInterface.h"
#include "SpecTclRESTInterface.h"
#include "Xamine2Root/HistFiller.h"
#include "dispshare.h"
#include "QRootCanvas.h"
#include "HistogramList.h"
#include "TCanvas.h"

#include <QMessageBox>

#include "Benchmark.h"
#include <chrono>

#include <iostream>

namespace Viewer {

SpecTclShMemInterface::SpecTclShMemInterface() :
    SpecTclInterface(),
    m_pRESTInterface(new SpecTclRESTInterface)
{

    Xamine_initspectra(); // sets up access to spectra

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
    Q_ASSERT( pCanvas != nullptr );

    // update all histograms in this canvas
    requestHistContentUpdate(pCanvas->getCanvas());
}

void SpecTclShMemInterface::requestHistContentUpdate(TPad *pPad)
{

    Benchmark<1, std::chrono::high_resolution_clock> bm;

    Q_ASSERT( pPad != nullptr );

    int padCount = 0;
    // update all histograms in this canvas
    auto pList = pPad->GetListOfPrimitives();
    TObject *pObject = nullptr;
    TIter it(pList);
    while (( pObject = it.Next() )) {
        if (pObject->InheritsFrom(TPad::Class()) && padCount < 1) {
            requestHistContentUpdate(dynamic_cast<TPad*>(pObject));
            ++padCount;
        } else if (pObject->InheritsFrom(TH1::Class())) {
            auto pHist = dynamic_cast<TH1*>(pObject);

            requestHistContentUpdate(QString(pHist->GetName()));
        }
    }
}

void SpecTclShMemInterface::requestHistContentUpdate(const QString &hName)
{
    Xamine2Root::HistFiller filler;

    HistogramBundle* pHBundle = getHistogramList()->getHist(hName);
    try {
        if (pHBundle) {
            filler.fill(*pHBundle->hist(), hName.toStdString());
        }
        emit histogramContentUpdated(pHBundle);
    } catch (std::exception& exc) {
        QMessageBox::warning(nullptr, QString("Histogram Update Error"), QString(exc.what()));
    }
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
