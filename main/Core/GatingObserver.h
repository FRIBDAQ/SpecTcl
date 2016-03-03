#ifndef GATINGOBSERVER_H
#define GATINGOBSERVER_H

class CGateContainer;
class CSpectrum;
class CHistogrammer;

/*!
 * \brief An observer for gating actions
 *
 * This is a purely virtual base class so it merely defines the interface
 * for derived classes to implement. The observer should be able to handle
 * when a gate is applied or removed from a spectrum.
 */
class CGatingObserver
{
public:
    virtual ~CGatingObserver() {};

    /*!
     * \brief Gate has been applied to the spectrum
     * \param rGate     - the gate
     * \param rSpectrum - the spectrum
     * \param rSorter   - access to the dictionaries
     */
    virtual void onApply(const CGateContainer& rGate, CSpectrum& rSpectrum,
                         CHistogrammer& rSorter) = 0;


    /*!
     * \brief Gate has been removed from the spectrum
     * \param rGate     - the gate
     * \param rSpectrum - the spectrum
     * \param rSorter   - access to the dictionaries
     */
    virtual void onRemove(const CGateContainer& rGate, CSpectrum& rSpectrum,
                          CHistogrammer& rSorter) = 0;

};

#endif // GATINGOBSERVER_H
