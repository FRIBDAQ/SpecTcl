#ifndef DOCKABLEGATEMANAGER_H
#define DOCKABLEGATEMANAGER_H

#include <QDockWidget>

class SpectrumViewer;
class TCutG;

namespace Ui {
class DockableGateManager;
}

class DockableGateManager : public QDockWidget
{

    Q_OBJECT
    
public:
    explicit DockableGateManager(const SpectrumViewer& viewer, QWidget *parent = 0);
    ~DockableGateManager();

public slots:
    void launchAddGateDialog();
    void registerGate(TCutG* pCut);

private:
    Ui::DockableGateManager *ui;
    const SpectrumViewer& m_view;
};

#endif // DOCKABLEGATEBUILDER_H
