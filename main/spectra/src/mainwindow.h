//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins 
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <memory>
#include <list>

namespace Ui {
class MainWindow;
}

namespace Viewer
{

class SpectrumView;
class HistogramView;
class DockableGateManager;
class SpecTclInterface;
class SpecTclInterfaceObserver;
class ControlPanel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void createDockWindows();

    void setSpecTclInterface(std::unique_ptr<SpecTclInterface> pInterface);

    void addSpecTclInterfaceObserver(std::unique_ptr<SpecTclInterfaceObserver> pObserver);

    void notifyObservers();

public slots:
    void onConnect();
    void dockHistograms();
    void dockGates();
    void onNewHistogram();

private:
    template<class T> void addGenericSpecTclInterfaceObserver(T& obj);



private:
    Ui::MainWindow                    *ui;
    SpectrumView                      *m_pView;
    HistogramView                     *m_histView;
    DockableGateManager               *m_gateView;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
    ControlPanel                      *m_pControls;
};



} // end of namespace
#endif // MAINWINDOW_H
