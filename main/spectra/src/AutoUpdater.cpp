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

#include "AutoUpdater.h"
#include "SpectrumView.h"
#include "SpecTclInterface.h"

#include <TH1.h>

#include <QTimer>

#include <vector>
#include <set>

namespace Viewer
{

AutoUpdater::AutoUpdater(std::shared_ptr<SpecTclInterface> pSpecTcl, SpectrumView &rView, QObject *parent)
    :   QObject(parent),
      m_pSpecTcl(pSpecTcl),
      m_pView(&rView),
      m_pTimer(new QTimer(this))
{
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

/*!
 * \brief AutoUpdater::onTimeout
 *
 *  This is a callback for when the wait period ends. It updates all of the spectra on
 *  the spectrum view that is assigned to this updated.
 */
void AutoUpdater::onTimeout()
{
    updateAll();
}

/*!
 * \brief AutoUpdater::start
 * \param nSeconds  number of seconds to timeout after
 *
 * Initiates the update mechanism. Once this is called, all previous timers
 * are stopped and a new one is started.
 */
void AutoUpdater::start(int nSeconds)
{
    // in case the timer is already active, we need to stop it to achieve
    // the correct period.
    m_pTimer->stop();
    m_pTimer->setInterval(nSeconds*1000);
    m_pTimer->start();
}

/*!
 * \brief AutoUpdater::stop
 *
 * Stopping the updater has no consequences besides cancelling the update. The currently
 * scheduled timer does not timeout and the spectra are not updated.
 */
void AutoUpdater::stop()
{
    m_pTimer->stop();
}


/*!
 * \brief AutoUpdater::getInterval
 *
 * \return  the update period in seconds
 */
int AutoUpdater::getInterval() const
{
    return m_pTimer->interval();
}

/*!
 * \brief AutoUpdater::isActive
 * \return boolean indicating whether updates are in process
 */
bool AutoUpdater::isActive() const
{
    return m_pTimer->isActive();
}

/*!
 * \brief AutoUpdater::updateAll
 *
 * Update all of the spectra that are displayed in the spectrum view.
 * Any histograms that may be found recursively are assumed to be found
 * by the SpectrumView::getAllHists method. This simply leans on that and
 * updates the histograms it receives. Note that this inherently causes
 * traffic through the SpecTclInterface.
 *
 * There is some intelligence built in to avoid making multiple requests for updates of
 * the same spectrum.
 */
void AutoUpdater::updateAll() {
    if (m_pSpecTcl) {
        auto canvases = m_pView->getAllCanvases();

        // this needs thread synchronization
        auto it = canvases.begin();
        auto it_end = canvases.end();
        std::set<TH1*> requestHistory;
        while ( it != it_end ) {
            std::vector<TH1*> hists = m_pView->getAllHists(*it);
            for (auto pHist : hists) {
                // try to insert into the request history.. if the hist already
                // lives in the request history, the second element of the
                // returned pair will be false
                if ( requestHistory.insert(pHist).second == true ) {
                    m_pSpecTcl->requestHistContentUpdate(QString(pHist->GetName()));
                }
            }
            ++it;
        }
    }
}

} // end Viewer namespace
