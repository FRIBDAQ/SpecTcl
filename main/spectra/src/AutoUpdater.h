#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include <QObject>

#include <memory>

class QTimer;

namespace Viewer
{

class SpecTclInterface;
class SpectrumView;


class AutoUpdater : public QObject
{
    Q_OBJECT

public:
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
