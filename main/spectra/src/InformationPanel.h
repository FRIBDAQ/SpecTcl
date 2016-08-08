#ifndef INFORMATIONPANEL_H
#define INFORMATIONPANEL_H

#include <memory>
#include <QWidget>

namespace Ui {
class InformationPanel;
}

namespace Viewer {

class HistogramBundle;
class GateManagerWidget;
class SpecTclInterface;
class SpectrumView;

class InformationPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit InformationPanel(SpectrumView& rView,
                              std::shared_ptr<SpecTclInterface> pSpecTcl,
                              const QString& histName,
                              QWidget *parent = 0);
    ~InformationPanel();
    
    void clearParameterList();
    void updateParameterList(HistogramBundle &rHist);
    void updateStatistics(HistogramBundle& rHist);
    void updateGates(HistogramBundle& rHist);

public slots:
    void onHistogramChanged(HistogramBundle& rHist);


private:
    void setUpStatisticsTable();
private:
    Ui::InformationPanel *ui;

    GateManagerWidget* m_pGateManager;
};

} // end of Viewer namespace
#endif // INFORMATIONPANEL_H
