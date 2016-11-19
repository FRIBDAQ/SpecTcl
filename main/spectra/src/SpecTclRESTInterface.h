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

#ifndef SPECTCLRESTINTERFACE_H
#define SPECTCLRESTINTERFACE_H

#include "SpecTclInterface.h"
#include "GateEditComHandler.h"
#include "CommonResponseHandler.h"
#include "GateListRequestHandler.h"
#include "MasterGateList.h"
#include "HistogramList.h"
#include "ListRequestHandler.h"
#include "ContentRequestHandler.h"

#include <memory>
#include <vector>

namespace SpJs {
  class GateInfo;
}

class TVirtualPad;

namespace Viewer
{

class GSlice;
class QRootCanvas;

/*! \brief Implementation of the SpecTcl facade for the REST server.
 *
 *  This provides a simple interface to the subsystem of objects that handle
 *  interactions with the REST server.
 */
class SpecTclRESTInterface : public SpecTclInterface
{
  Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Creates a new HistogramList, GateList, and request handlers.
     */
    SpecTclRESTInterface();
    virtual ~SpecTclRESTInterface();

    ///////////////////////////////////////////////////////////////////////////
    // Gate operations
    /*!
     * \brief Send REST request to add a new 1D gate
     *
     * \param slice     the slice to add
     */
    void addGate(const GSlice& slice);

    /*!
     * \brief Send REST request to modify a 1D gate
     *
     * \param slice     the slice to edit
     */
    void editGate(const GSlice& slice);

    /*!
     * \brief Send REST request to "delete" a 1D gate
     *
     * \param slice     the slice to delete
     */
    void deleteGate(const GSlice& slice);

    /*!
     * \brief Send REST request to add a new 2D gate
     *
     * \param gate  the gate to add
     */
    void addGate(const GGate& slice);

    /*!
     * \brief Send REST request to edit a new 2D gate
     *
     * \param gate  the gate to edit
     */
    void editGate(const GGate& slice);
    /*!
     * \brief Send REST request to delete a new 2D gate
     *
     * \param gate the gate to delete
     */
    void deleteGate(const GGate& slice);

    /*!
     * \brief Send REST request to delete a generic gate
     *
     * \param name name of gate to delete
     */
    void deleteGate(const QString& name);

    /*!
     * \brief enableGatePolling
     *
     * \param enable  turns on (true) or off (false) polling
     */
    void enableGatePolling(bool enable);

    /*!
     * \brief Check whether gate polling is enabled
     *
     * \returns boolean
     * \retval true - polling active
     * \retval false - polling disabled
     */
    bool gatePollingEnabled() const { return pollGates;}

    /*!
     * \brief Retrieve the list of gates in SpecTcl
     *
     * \return pointer to the gate list for this SpecTcl interface
     */
    MasterGateList* getGateList() { return m_pGateList.get(); }

    ///////////////////////////////////////////////////////////////////////////
    // Histogram (Spectrum) operations

    /*!
     * \brief Enable Histogram Info Polling
     *
     * The caller can use this to turn polling on or off. If being turned off,
     * the last active request will be allowed to complete, but will not be
     * rescheduled if the user chooses not to.
     *
     * \param enable  whether to enable or disable polling
     */
    void enableHistogramInfoPolling(bool enable);

    /*!
     * \brief Check whether hist info polling is enabled
     *
     * \return boolean
     * \retval true - polling is active
     * \retval false - polling is inactive
     */
    bool histogramInfoPollingEnabled() const { return pollHistInfo;}

    /*!
     * \brief Retrieve the histogram list
     *
     * \return pointer to histogram list for this interface
     */
    HistogramList* getHistogramList() { return m_pHistList.get(); }

    /*!
     * \brief Request content updates for all histograms in a canvas
     *
     * \param pCanvas - the canvas containing the histogram
     */
    void requestHistContentUpdate(QRootCanvas *pView);

    /*!
     * \brief Request content updates for all histograms in a pad
     * \param pPad - the pad containing the histograms
     */
    void requestHistContentUpdate(TVirtualPad *pPad);

    /*!
     * \brief Request content updates for a specific histogram
     *
     * This accesses histogram content via the REST interface.
     *
     * \param hName - name of histogram to update
     */
    void requestHistContentUpdate(const QString& hName);

public slots:
    void listGates();
    void listHistogramInfo();
    void onGateListReceived(std::vector<SpJs::GateInfo*> gates);
    void onHistogramListReceived(std::vector<SpJs::HistInfo> hists);
    void onHistogramContentUpdated(HistogramBundle* pBundle);

private:
    std::unique_ptr<MasterGateList> m_pGateList;
    std::unique_ptr<HistogramList> m_pHistList;
    std::unique_ptr<GateEditComHandler> m_pGateEditCmd;
    std::unique_ptr<CommonResponseHandler> m_pCommonHandler;
    std::unique_ptr<GateListRequestHandler> m_pGateListCmd;
    std::unique_ptr<ListRequestHandler> m_pHistListCmd;
    std::unique_ptr<ContentRequestHandler> m_pHistContentCmd;
    bool pollGates;
    bool pollHistInfo;
};

} // end of namespace

#endif // SPECTCLRESTINTERFACE_H
