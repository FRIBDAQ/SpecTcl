#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QRootCanvas;

class TH1;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QRootCanvas *m_canvas;
    TH1* m_hist;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void update();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
