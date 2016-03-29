#ifndef INFORMATIONPANEL_H
#define INFORMATIONPANEL_H

#include <QWidget>

namespace Ui {
class InformationPanel;
}

namespace Viewer {

class HistogramBundle;

class InformationPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit InformationPanel(QWidget *parent = 0);
    ~InformationPanel();
    
public slots:
    void onHistogramChanged(HistogramBundle& rHist);

private:
    Ui::InformationPanel *ui;
};

} // end of Viewer namespace
#endif // INFORMATIONPANEL_H
