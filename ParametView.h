#ifndef PARAMETVIEW_H
#define PARAMETVIEW_H

#include <QDockWidget>

namespace Ui {
class DockWidget;
}

class DockWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit DockWidget(QWidget *parent = 0);
    ~DockWidget();
    
private:
    Ui::DockWidget *ui;
};

#endif // PARAMETVIEW_H
