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

#ifndef HISTOGRAMBUNDLE_H
#define HISTOGRAMBUNDLE_H

#include <SubscribableH1.h>

#include "QHistInfo.h"

#include "TH1.h"

#include <QMutex>
#include <QString>
#include <QMap>

#include <map>
#include <memory>

class TVirtualPad;

namespace Viewer
{

class GGate;
class GSlice;
class MasterGateList;

/*! \brief Collection of a histogram and its cuts
 *
 * We have a histogram centric view of the world here. User's can associate
 * an arbitrary number of cuts to a
 * histogram, though you are limited to 2d cuts on 2d histograms
 * and 1d cuts on 1d histograms. Histograms can only ever have 1 cut applied
 * to it at a time, however, we always show all cuts associated with the
 * parameters.
 *
 * Histogram bundles are used on different threads and must be handled
 * appropriately. There is a mutex owned by each HistogramBundle and it
 * should be locked when trying to read or write the state of the bundle.
 *
 * This class assumes that there is always a TH1 object. It should never be nullptr.
 */
class HistogramBundle {

private:
  std::unique_ptr<QMutex>       m_pMutex;
  std::unique_ptr<TH1>          m_pHist;
  std::map<QString, GSlice*>    m_cuts1d;
  std::map<QString, GGate*>     m_cuts2d;
  SpJs::HistInfo                m_hInfo;
  QString						m_defaultDrawOption;
  std::map<TVirtualPad*,TH1*>       m_clones;
  GenericH1Subscriber<HistogramBundle> m_subscriber;

public:
    /*!
     * \brief HistogramBundle
     */
    HistogramBundle(std::unique_ptr<QMutex> pMutex,
                    std::unique_ptr<TH1> pHist, const SpJs::HistInfo& info);

    virtual ~HistogramBundle();

    ///////////////////////////////////////////////////////////////////////////
    // Mutex methods

    QMutex* getMutex() { return m_pMutex.get(); }
    void lock() const { m_pMutex->lock(); }
    void unlock() const { m_pMutex->unlock(); }

    ///////////////////////////////////////////////////////////////////////////
    // Simple Getters
    TH1&            getHist() const { return *m_pHist; }
    SpJs::HistInfo  getInfo() const { return m_hInfo; }
    QString         getName() const { return QString::fromStdString(m_hInfo.s_name); }

    ///////////////////////////////////////////////////////////////////////////

    /*!
     * \brief Add a 1D cut to the local gate list
     *
     * \param pSlice  pointer to cut
     *
     * Note that the pointer is stored and no ownership is claimed.
     */
    void addCut1D(GSlice* pSlice);

    /*!
     * \brief Add a 2D cut to the local gate list
     *
     * \param pCut  pointer to cut
     *
     * Note that the pointer is stored and no ownership is claimed.
     */
    void addCut2D(GGate* pCut);

    void clearCut1Ds() { m_cuts1d.clear(); }
    void clearCut2Ds() { m_cuts2d.clear(); }

    std::map<QString, GSlice*>& getCut1Ds() { return m_cuts1d; }
    std::map<QString, GSlice*>  getCut1Ds() const { return m_cuts1d; }

    std::map<QString, GGate*>& getCut2Ds() { return m_cuts2d; }
    std::map<QString, GGate*>  getCut2Ds() const { return m_cuts2d; }

    /*!
     * \brief Synchronize local list of gate with the master gate list
     *
     * \param pGateList     the master gate list
     * \return boolean
     * \retval true  - local gate list was updated
     * \retval false - local gate list was already up to date.
     */
    bool synchronizeGates(const MasterGateList* pGateList);

    /*!
     * \brief Adds the histogram to the current canvas as a drawable entity
     *
     * \param opt - the draw option
     *
     * Different default options are provided for the 1d and 2d histograms.
     * If the caller specifies an option, then that option supercedes the default
     * option.
     */
    void draw(const QString& opt = QString());


    void setDefaultDrawOption(const QString& opt);
    QString getDefaultDrawOption() const;

    std::map<TVirtualPad*,TH1*> getClones() { return m_clones; }

    /*!
     * \brief Respond to clone deletion notificiation
     *
     * Search for and remove the histogram from the list of clones.
     * This ensures that drawn objects don't proliferate and use
     * resources during copy operations.
     *
     * \param hist  the histogram that is being deleted
     */
    void notify(TH1& hist);

    //////////////////////////////////////////////////////////////////////////
    // Helper methods
private:
    bool synchronize1DGates(const MasterGateList* pGateList);
    bool synchronize2DGates(const MasterGateList* pGateList);
};

} // end of namespace

/*!
 * \brief Insertion operator overload
 *
 * \param stream    a output stream
 * \param hist      histogram bundle
 *
 * This is mainly for debugging purposes. It prints a textual representation
 * of the bundle.
 *
 * \return the output stream
 */
extern std::ostream& operator<<(std::ostream& stream,
                                const Viewer::HistogramBundle& hist);

#endif // HISTOGRAMBUNDLE_H
