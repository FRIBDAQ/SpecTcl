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
