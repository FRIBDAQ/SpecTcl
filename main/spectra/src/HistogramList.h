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

#ifndef HISTOGRAMLIST_H
#define HISTOGRAMLIST_H

#include "HistogramBundle.h"
#include "MasterGateList.h"

#include <QFrame>
#include <QString>

#include <memory>
#include <map>

class TH1;

namespace SpJs {
    class HistInfo;
}

namespace Viewer
{

  class SpecTclInterface;

/*! \brief Owner of all the HistogramBundles in the application.
 *
 * THis class is responsible for managing/controlling access to the histogram
 * bundles. The list is used in multiple threads and a mutex is provided to
 * manage access. Tehre is no internal logic to lock/unlock the mutex during operations
 * and it is the callers responsibility to lock/unlock it. Users should recognize that
 * each HistogramBundle also has its own mutex that should be dealt with appropriately.
 *
 * This class also provides some utility methods for performing operations on the
 * entire list. These operations typically involve the manipulation of gates because whenever
 * a new gate is added, it is required that all of the histograms associated with it are
 * updated.
 *
 * HistogramBundles can be looked up by name.
 */
class HistogramList : public QObject
{
    Q_OBJECT
    
    //////////////////////////////////////////////////////////////////////////

public:
    using Container      = std::map<QString, std::unique_ptr<HistogramBundle> >;
    using iterator       = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;

    explicit HistogramList(QObject *parent = 0);
    HistogramList(const HistogramList&) = delete;
    ~HistogramList();


    //////////////////////////////////////////////////////////////////////////
    // Mutex operations

    /*!
     * \brief This returns a pointer to the mutex out of convenience.
     * \return non-null pointer to mutex
     */
    QMutex* getMutex() { return &m_mutex; }

    /*!
     * \brief Lock the mutex for the list
     */
    void lock() { m_mutex.lock(); }

    /*!
     * \brief Unlock the mutex for the list
     */
    void unlock() { m_mutex.unlock(); }

    //////////////////////////////////////////////////////////////////////////
    // List operations

    /*!
     * \brief Remove/delete all histogram bundles
     */
    void clear();

    /*!
     * \return the number of histogram bundles
     */
    std::map<QString, HistogramBundle>::size_type
    size() { return m_hists.size(); }

    /*!
     * \brief Iterator access
     * \return iterator to first HistogramBundle of list
     */
    iterator begin() { return m_hists.begin();}

    /*!
     * \brief Iterator access
     * \return iterator just beyond last HistogramBundle of list
     */
    iterator end() { return m_hists.end();}


    /*!
     * \brief Soft insert of a histogram bundle (may fail silently)
     * \param hist  the histogram
     * \param info  the information
     *
     * The ownership of the histogram transfers to the list. The
     * caller should use the returned pointer to access the histogram bundle
     * in the future. If a histogram of the same name already exists, then this
     * returns the prexisting histogram bundle. In that case, the bundle passed in
     * will implicitly deleted.
     *
     * \todo Fix the semantics for inserting a new histogram
     *
     * \return pointer to the stored histogram bundle
     */
    HistogramBundle* addHist(std::unique_ptr<TH1> hist, const SpJs::HistInfo& info);

    /*!
     * \brief Insert a histogram bundle
     *
     * \param hist  histogram to insert
     *
     * Contrary to the other addHist method, this will replace any existing histogram stored with
     * a matching name unconditionally.
     *
     * \return pointer to the histogram bundle added
     */
    HistogramBundle* addHist(std::unique_ptr<HistogramBundle> hist);

    //////////////////////////////////////////////////////////////////////////
    // Methods to manipulate gates

    /*!
     * \brief Remove gates from all histograms
     */
    void clearCuts();

    /*!
     * \brief All histogram bundles synchronize to the master gate list
     * \param pList the master gate list
     */
    void synchronize(const MasterGateList& pList);

    /*!
     * \brief Synchronize histogram bundles for 1d gates
     */
    void synchronize1d(MasterGateList::iterator1d begin, MasterGateList::iterator1d end);

    /*!
     * \brief Synchronize histogram bundles for 2d gates
     */
    void synchronize2d(MasterGateList::iterator2d begin, MasterGateList::iterator2d end);

    /*!
     * \brief Synchronize list to a list of histogram infos
     *
     * \param hists     information about histograms that exist in SpecTcl
     *
     * \return boolean
     * \retval true    local list was changed to bring it up to date
     * \retval false   local list already up to date
     */
    bool update(const std::vector<SpJs::HistInfo>& hists);

    //////////////////////////////////////////////////////////////////////////

    /*!
     * \brief Look up a histogram bundle owning the TH1 object
     *
     * \param hist  pointer to histogram to look for
     *
     * \return HistogramBundle*
     * \retval nullptr - if not found
     * \retval pointer to histogram bundle that owns the TH1 object
     */
    HistogramBundle* getHist(const TH1* hist);

    /*!
     * \brief Look up a histogram bundle by name
     *
     * \param name      name of the bundle
     *
     * \return HistogramBundle*
     * \retval nullptr      if not found
     * \retval pointer to histogram bundle that owns the TH1 object otherwise
     */
    HistogramBundle* getHist(const QString& name);

    /*!
     * \return Retrieve the list of histogram names in list
     */
    QList<QString> histNames();

public slots:

    /*!
     * \brief Check whether the histogram exists
     * \param name  name of histogram to look for
     * \return boolean
     * \retval true  histogram exists
     * \retval false otherwise
     */
    bool histExists(const QString& name);

    /*!
     * \brief Remove 1d gate from all histograms
     * \param slice 1d gate to remove
     */
    void removeSlice(const GSlice& slice);

    /*!
     * \brief Remove 2d gate from all histograms
     * \param gate 2d gate to remove
     */
    void removeGate(const GGate& gate);

    /*!
     * \brief Add 1d gate to all histograms that are associated with it
     * \param slice     1d gate to add
     */
    void addSlice(GSlice* slice);

    /*!
     * \brief Add 2d gate to all histograms that are associated with it
     * \param gate      2d gate to add
     */
    void addGate(GGate* gate);

signals:
        void histogramRemoved(HistogramBundle*);

private:
    Container  m_hists;
    QMutex     m_mutex;
};

} // end of namespace

#endif // HISTOGRAMLIST_H
