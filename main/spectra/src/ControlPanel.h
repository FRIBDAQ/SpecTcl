#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>

#include <memory>

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
    explicit ControlPanel(std::shared_ptr<SpecTclInterface> pSpecTcl,
                          SpectrumView* pView, QWidget *parent = 0);
    ~ControlPanel();

    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);

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
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
    SpectrumView* m_pView;
};

} // end of namespace

#endif // CONTROLPANEL_H
