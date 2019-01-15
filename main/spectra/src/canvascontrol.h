#ifndef CANVASCONTROL_H
#define CANVASCONTROL_H

#include <QFrame>

namespace Ui {
class CanvasControl;
}

class CanvasControl : public QFrame
{
    Q_OBJECT

public:
    explicit CanvasControl(QWidget *parent = 0);
    ~CanvasControl();

private slots:
    void on_pClearSelected_clicked();

    void on_pClearAll_clicked();

    void on_pClearVisible_clicked();

    void on_pEmptySelected_clicked();

    void on_pEmptyVisible_clicked();

private:
    Ui::CanvasControl *ui;
};

#endif // CANVASCONTROL_H
