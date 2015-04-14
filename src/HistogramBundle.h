//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
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

#ifndef HISTOGRAMBUNDLE_H
#define HISTOGRAMBUNDLE_H

#include <QMutex>
#include <QString>
#include <HistInfo.h>
#include <vector>

class TH1;
class GGate;
class GSlice;


/*! \brief Collection of a histogram and its cuts
 *
 * We have a histogram centric view of the world here. 
 * User's can associate an arbitrary number of cuts to a 
 * histogram, though you are limited to 2d cuts on 2d histograms
 * and 1d cuts on 1d histograms.
 *
 * Histogram bundles are thread safe and own a mutex that is to
 * be used as a synchronization mechanism. There are convenience
 * methods that allow you to lock the bundle and unlock it.
 *
 */
class HistogramBundle {
private:
    QMutex* m_pMutex;
    TH1* m_pHist;
    std::vector<GGate*> m_cuts2d;
    std::vector<GSlice*> m_cuts1d;
    SpJs::HistInfo m_hInfo;

public:
    HistogramBundle();
    HistogramBundle(QMutex& pMutex, TH1& pHist, const SpJs::HistInfo& info);

    // Mutex methods
    void lock() const { m_pMutex->lock(); }
    void unlock() const { m_pMutex->unlock(); }

    // Getters
    TH1* hist() const { return m_pHist; }
    SpJs::HistInfo getInfo() const { return m_hInfo; }

    // Add cuts
    void addCut1D(GSlice* pSlice);
    void addCut2D(GGate* pCut);

    // Draw the histogram.
    void draw(const QString& opt = QString());
};


#endif // HISTOGRAMBUNDLE_H
