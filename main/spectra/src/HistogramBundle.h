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

#include "QHistInfo.h"
#include "TH1.h"

#include <QMutex>
#include <QString>
#include <map>

#include <memory>



namespace Viewer
{

class GGate;
class GSlice;
class GateList;

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
  std::unique_ptr<QMutex> m_pMutex;
  std::unique_ptr<TH1> m_pHist;
  std::map<QString, GSlice*> m_cuts1d;
  std::map<QString, GGate*> m_cuts2d;
  SpJs::HistInfo m_hInfo;

public:
    HistogramBundle();
    HistogramBundle(std::unique_ptr<QMutex> pMutex, 
                    std::unique_ptr<TH1> pHist, const SpJs::HistInfo& info);

    // Mutex methods
    QMutex* getMutex() { return m_pMutex.get(); }
    void lock() const { m_pMutex->lock(); }
    void unlock() const { m_pMutex->unlock(); }

    // Getters
    TH1* hist() const { return m_pHist.get(); }
    SpJs::HistInfo getInfo() const { return m_hInfo; }

    QString getName() const { return QString::fromStdString(m_hInfo.s_name); }

    // Add cuts
    void addCut1D(GSlice* pSlice);
    void addCut2D(GGate* pCut);

    void clearCut1Ds() { m_cuts1d.clear(); }
    void clearCut2Ds() { m_cuts2d.clear(); }

    std::map<QString, GSlice*>& getCut1Ds() { return m_cuts1d; }
    std::map<QString, GSlice*> getCut1Ds() const { return m_cuts1d; }

    std::map<QString, GGate*>& getCut2Ds() { return m_cuts2d; }
    std::map<QString, GGate*> getCut2Ds() const { return m_cuts2d; }

    // Draw the histogram.
    void draw(const QString& opt = QString());

    bool synchronizeGates(const GateList* pGateList);
};

} // end of namespace


extern std::ostream& operator<<(std::ostream& stream, const Viewer::HistogramBundle& hist);

#endif // HISTOGRAMBUNDLE_H
