#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include <QObject>

#include <memory>

class QTimer;

namespace Viewer
{

class SpecTclInterface;
class SpectrumView;

/*!
 * \brief The AutoUpdater class
 *
 * This provides facilities to periodically update a spectrum view automatically.
 * The user sets this up through a drop down menu. Currently, there is a one-to-one
 * relationship between a SpectrumView and an AutoUpdater class. The TabWorkspace
 * is what orchestrates this, as it owns both of them.
 *
 * At the moment, the AutoUpdate feature only supports the updating of all spectra
 * on a single tab at the same time.
 *
 */
class AutoUpdater : public QObject
{
    Q_OBJECT

public:

    //
    // For documentation, please see the doxygen generated docs or the source code
    //

    explicit AutoUpdater(std::shared_ptr<SpecTclInterface> pSpecTcl,
                         SpectrumView& rView,
                         QObject *parent = 0);

    void start(int nSeconds);
    void stop();

    int  getInterval() const;
    bool isActive() const;
    void updateAll();

public slots:
    void onTimeout();

private:
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
    SpectrumView*                     m_pView;
    QTimer*                           m_pTimer;
};

} // end Viewer namespace

#endif // AUTOUPDATER_H
