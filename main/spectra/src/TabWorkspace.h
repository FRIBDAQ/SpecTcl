#ifndef TABWORKSPACE_H
#define TABWORKSPACE_H

#include <QWidget>
#include <QStringList>

#include <memory>

namespace Viewer
{

class SpectrumView;
class ViewDrawPanel;
class ControlPanel;
class SpecTclInterface;
class MultiInfoPanel;

class TabWorkspace : public QWidget
{
    Q_OBJECT

public:
    explicit TabWorkspace(std::shared_ptr<SpecTclInterface> pSpecTcl, QWidget *parent = 0);
    
    void layoutSpectra(QStringList spectrumList);

signals:
    
public slots:
    void showHideStatistics();
    void showHideDrawPanel();

private:
    void setUpUI();
    void connectSignals();

private:
    std::shared_ptr<SpecTclInterface>   m_pSpecTcl;
    SpectrumView*                       m_pView;
    ViewDrawPanel*                      m_pDrawPanel;
    ControlPanel*                       m_pControls;
    MultiInfoPanel*                     m_pInfoPanel;
};

} // end Viewer namespace

#endif // TABWORKSPACE_H
