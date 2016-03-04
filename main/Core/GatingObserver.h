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
