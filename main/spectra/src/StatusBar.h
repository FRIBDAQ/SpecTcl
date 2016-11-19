#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QWidget>



namespace Ui {
class StatusBar;
}

namespace Viewer
{

class StatusBar : public QWidget
{
    Q_OBJECT
    
public:
    explicit StatusBar(QWidget *parent = 0);
    ~StatusBar();
    
public slots:
    void onCursorMoved(const char* pStatus);

private:
    Ui::StatusBar *ui;
};

} // end Viewer namespace

#endif // STATUSBAR_H
