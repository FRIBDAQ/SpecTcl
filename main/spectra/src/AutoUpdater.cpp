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

void AutoUpdater::onTimeout()
{
    updateAll();
}

void AutoUpdater::start(int nSeconds)
{
    // in case the timer is already active, we need to stop it to achieve
    // the correct period.
    m_pTimer->stop();
    m_pTimer->setInterval(nSeconds*1000);
    m_pTimer->start();
}

void AutoUpdater::stop()
{
    m_pTimer->stop();
}

int AutoUpdater::getInterval() const
{
    return m_pTimer->interval();
}

bool AutoUpdater::isActive() const
{
    return m_pTimer->isActive();
}

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
