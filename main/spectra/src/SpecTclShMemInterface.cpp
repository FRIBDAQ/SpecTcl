//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#include "SpecTclShMemInterface.h"
#include "SpecTclRESTInterface.h"
#include "Xamine2Root/HistFiller.h"
#include "dispshare.h"
#include "QRootCanvas.h"
#include "HistogramList.h"
#include "CanvasOps.h"

#include "TCanvas.h"

#include <QMessageBox>

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

    auto histNames = CanvasOps::extractAllHistNames(*pCanvas);

    for (auto& name : histNames) {
        // update all histograms in this canvas
        requestHistContentUpdate(name);
    }
}

void SpecTclShMemInterface::requestHistContentUpdate(TPad *pPad)
{
    Q_ASSERT( pPad != nullptr );

    auto histNames = CanvasOps::extractAllHistNames(*pPad);

    for (auto& name : histNames) {
        // update all histograms in this canvas
        requestHistContentUpdate(name);
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
