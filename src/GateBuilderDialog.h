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

#ifndef GATEBUILDERDIALOG_H
#define GATEBUILDERDIALOG_H

#include "HistogramBundle.h"
#include <QDialog>
#include <QString>
#include <TCutG.h>
#include <memory>
#include <QButtonGroup>

class TPad;
class GGate;
class QRootCanvas;

namespace Ui {
class GateBuilderDialog;
}

class GateBuilderDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GateBuilderDialog(QRootCanvas& viewer,
                               HistogramBundle& hist,
                               GGate* pCut=nullptr,
                               QWidget *parent = 0);
    ~GateBuilderDialog();

    void setCutName(const QString& name);
    void setCut(GGate* pCut);

public slots:
    virtual void accept();
    virtual void reject();

    void newPoint(TPad* pad);
    void onNameChanged(const QString& name);
    void onTypeChanged(int type);

signals:
    void completed(GGate* pCut);

private:
    void hideOldCut(GGate& gate);
    void appendPointToTable(double x, double y);
    void appendPointToCut(double x, double y);
    void fillTableWithData(GGate& rCut);
    void ensureLastPointDiffersFromFirst();
    void ensureLastPointMatchesFirst();



private:
    Ui::GateBuilderDialog *ui;
    QRootCanvas& m_canvas;
    HistogramBundle& m_histPkg;
    QString m_name;
    std::unique_ptr<GGate> m_pEditCut;
    GGate* m_pOldCut;
    QButtonGroup m_radioButtons;
    bool m_matchLast;
};

#endif // GATEBUILDERDIALOG_H
