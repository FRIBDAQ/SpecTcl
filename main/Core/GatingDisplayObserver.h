#ifndef GATINGDISPLAYOBSERVER_H
#define GATINGDISPLAYOBSERVER_H

#include "GatingObserver.h"

class CDisplayInterface;


/*!
 * \brief The CGatingDisplayObserver class
 *
 * An observer of the application of gates. This is mainly responsible for
 * updating the info of the spectra when a gate in applied.
 */
class CGatingDisplayObserver : public CGatingObserver
{
private:
    CDisplayInterface* m_pInterface;

public:
    /*!
     * \brief Constructor
     *
     * \param pInterface    the display interface
     */
    CGatingDisplayObserver(CDisplayInterface* pInterface);

    /*!
     * \brief Updates the title and info for a bound spectrum
     *
     * \param rGate     - the newly applied gate
     * \param rSpectrum - the spectrum
     * \param rSorter   - histogrammer for access to dictionaries
     *
     * Title is set to the name of the spectrum. The info is set to
     * whatever the display will choose given the informatoin passed
     * in.
     */
    void onApply(const CGateContainer &rGate,
                 CSpectrum &rSpectrum,
                 CHistogrammer& rSorter);

    /*!
     * \brief Not used.
     */
    void onRemove(const CGateContainer &rGate,
                  CSpectrum &rSpectrum,
                  CHistogrammer& rSorter);
};

#endif // GATINGDISPLAYOBSERVER_H
