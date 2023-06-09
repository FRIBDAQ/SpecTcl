#ifndef MULTIINFOPANEL_H
#define MULTIINFOPANEL_H

#include <QWidget>

#include <memory>

namespace Ui {
class MultiInfoPanel;
}

namespace Viewer {

class QRootCanvas;
class SpecTclInterface;
class HistogramBundle;
class TabWorkspace;

/*!
 * \brief The MultiInfoPanel class
 *
 * The MultiInfoPanel class is a tab widget that contains multiple
 * InformationalPanels. There is a tab for each histogram that is displayed
 * on the canvas.
 */
class MultiInfoPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit MultiInfoPanel(TabWorkspace& rView,
                            std::shared_ptr<SpecTclInterface> pSpecTcl,
                            QWidget *parent = 0);
    ~MultiInfoPanel();

    int getNumberOfTabs() const { return m_nValidTabs; }
    
    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);

public slots:
    void currentCanvasChanged(QRootCanvas& rCanvas);
    void updateContent(QRootCanvas& rCanvas);

private:
    void addEmptyTab();
    void addTab(HistogramBundle& rBundle);
    void removeAllTabs();
    int findTab(const QString& name);

private:
    Ui::MultiInfoPanel                  *ui;
    TabWorkspace&                     m_workspace;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
    int                               m_nValidTabs;
};

} // end Viewer namespace

#endif // MULTIINFOPANEL_H
