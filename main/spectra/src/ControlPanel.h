#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>

namespace Ui {
class ControlPanel;
}

namespace Viewer
{
  class GeometrySelector;
  class SpecTclInterface;
  class SpectrumView;

class ControlPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit ControlPanel(SpecTclInterface* pSpecTcl, SpectrumView* pView, QWidget *parent = 0);
    ~ControlPanel();

public slots:
    void onUpdateSelected();
    void onUpdateAll();
    void onColumnCountChanged(int);
    void onRowCountChanged(int);
    void onRefresh();

signals:
    void updateSelected();
    void updateAll();
    void geometryChanged(int nRows, int nCols);

private:
    Ui::ControlPanel *ui;
    GeometrySelector* m_pGeoSelector;
    SpecTclInterface* m_pSpecTcl;
    SpectrumView* m_pView;
};

} // end of namespace

#endif // CONTROLPANEL_H
