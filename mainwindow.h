#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class SpectrumViewer;
class HistogramView;
class DockableGateManager;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void createDockWindows();

public slots:
    void onConnect();
    void dockHistograms();
    void dockGates();
    void onNewHistogram();

private:
    Ui::MainWindow *ui;
    SpectrumViewer* m_view;
    HistogramView* m_histView;
    DockableGateManager* m_gateView;
};

#endif // MAINWINDOW_H
