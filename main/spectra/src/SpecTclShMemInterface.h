#ifndef VIEWER_SPECTCLSHMEMINTERFACE_H
#define VIEWER_SPECTCLSHMEMINTERFACE_H

#include "SpecTclRESTInterface.h"

#include <memory>

namespace Viewer {

/*!
 * \brief The SpecTclShMemInterface class
 *
 * This is an implementation of the SpecTclInterface that supports
 * communication over both the REST server and also through shared memory.
 * All communication goes through the REST server unless it is a histogram contnet
 * request. In that case, the Xamine shared memory interface is used to request histogram
 * content.
 *
 * This is used in the "Local" mode of Spectra. It mainly delegates its work to an
 * instance of SpecTclRESTInterface that it owns.
 *
 */
class SpecTclShMemInterface : public Viewer::SpecTclInterface
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Sets up the shared memory for accessing spectra and creates the
     * SpecTclRESTInterface.
     */
    SpecTclShMemInterface();

    /*!
     * \brief Destructor - doesn't do anything special.
     */
    virtual ~SpecTclShMemInterface();

public:
    ///////////////////////////////////////////////////////////////////////////
    // Gate operations

    /*!
     * \brief Send REST request to add a new 1D gate
     *
     * \param slice     the slice to add
     */
    virtual void addGate(const GSlice& slice);

    /*!
     * \brief Send REST request to modify a 1D gate
     *
     * \param slice     the slice to edit
     */
    virtual void editGate(const GSlice &slice);

    /*!
     * \brief Send REST request to "delete" a 1D gate
     *
     * \param slice     the slice to delete
     */
    virtual void deleteGate(const GSlice &slice);

    /*!
     * \brief Send REST request to add a new 2D gate
     *
     * \param gate  the gate to add
     */
    virtual void addGate(const GGate& gate);

    /*!
     * \brief Send REST request to edit a new 2D gate
     *
     * \param gate  the gate to edit
     */
    virtual void editGate(const GGate& gate);

    /*!
     * \brief Send REST request to delete a new 2D gate
     *
     * \param gate the gate to delete
     */
    virtual void deleteGate(const GGate& gate);

    /*!
     * \brief Send REST request to delete a generic gate
     *
     * \param name name of gate to delete
     */
    virtual void deleteGate(const QString& name);

    /*!
     * \brief enableGatePolling
     *
     * \param enable  turns on (true) or off (false) polling
     */
    virtual void enableGatePolling(bool enable);

    /*!
     * \brief Check whether gate polling is enabled
     *
     * \returns boolean
     * \retval true - polling active
     * \retval false - polling disabled
     */
    virtual bool gatePollingEnabled() const;

    /*!
     * \brief Retrieve the list of gates in SpecTcl
     *
     * \return pointer to the gate list for this SpecTcl interface
     */
    virtual GateList* getGateList();

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
    virtual void enableHistogramInfoPolling(bool enable);

    /*!
     * \brief Check whether hist info polling is enabled
     *
     * \return boolean
     * \retval true - polling is active
     * \retval false - polling is inactive
     */
    virtual bool histogramInfoPollingEnabled() const;

    /*!
     * \brief Retrieve the histogram list
     *
     * \return pointer to histogram list for this interface
     */
    virtual HistogramList* getHistogramList();

    /*!
     * \brief Request content updates for all histograms in a canvas
     *
     * \param pCanvas - the canvas containing the histogram
     */
    virtual void requestHistContentUpdate(QRootCanvas *pCanvas);

    /*!
     * \brief Request content updates for all histograms in a pad
     * \param pPad - the pad containing the histograms
     */
    virtual void requestHistContentUpdate(TPad* pPad);

    /*!
     * \brief Request content updates for a specific histogram
     *
     * This accesses histogram content via shared memory
     *
     * \param hName - name of histogram to update
     */
    virtual void requestHistContentUpdate(const QString& hName);

    ///////////////////////////////////////////////////////////////////////////
    // Slot
    ///////////////////////////////////////////////////////////////////////////
public slots:
    void onHistogramContentUpdated(HistogramBundle *pBundle);
    void onHistogramListChanged();
    void onGateListChanged();

private:
    std::unique_ptr<SpecTclRESTInterface> m_pRESTInterface; ///< REST interface
};

} // namespace VIewer

#endif // VIEWER_SPECTCLSHMEMINTERFACE_H
