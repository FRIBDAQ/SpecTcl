#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>

namespace Ui {
class ControlPanel;
}

namespace Viewer
{

class ControlPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit ControlPanel(QWidget *parent = 0);
    ~ControlPanel();

public slots:
    void onUpdateSelected();
    void onUpdateAll();
    void onUpdateGeometry();

signals:
    void updateSelected();
    void updateAll();
    void updateGeometry(int nRows, int nCols);

private:
    Ui::ControlPanel *ui;
};

} // end of namespace

#endif // CONTROLPANEL_H
