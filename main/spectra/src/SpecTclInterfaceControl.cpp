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


#include "SpecTclInterfaceControl.h"

#include "SpecTclInterface.h"

namespace Viewer {

//
//
SpecTclInterfaceControl::SpecTclInterfaceControl(std::shared_ptr<SpecTclInterface> pInterface)
    : m_pInterface(pInterface),
      m_interfaceObservers()
{
}


//
//
void SpecTclInterfaceControl::setInterface(std::shared_ptr<SpecTclInterface> pInterface)
{
    bool gatePollingEnabled = true;
    bool histPollingEnabled = true;

    if (m_pInterface) {
        gatePollingEnabled = m_pInterface->gatePollingEnabled();
        histPollingEnabled = m_pInterface->histogramInfoPollingEnabled();
    }

    m_pInterface = pInterface;

    m_pInterface->enableGatePolling(gatePollingEnabled);
    m_pInterface->enableHistogramInfoPolling(histPollingEnabled);

    notifyObservers();
}

//
void
SpecTclInterfaceControl::addSpecTclInterfaceObserver(std::unique_ptr<SpecTclInterfaceObserver> pObserver)
{
    m_interfaceObservers.insert(m_interfaceObservers.end(), std::move(pObserver));
}

//
//
void SpecTclInterfaceControl::notifyObservers()
{
    for(auto& pObserver : m_interfaceObservers) {
        pObserver->update(m_pInterface);
    }
}

} // namespace Viewer
