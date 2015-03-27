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
class TCutG;
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
                               TCutG* pCut=nullptr,
                               QWidget *parent = 0);
    ~GateBuilderDialog();

    void setCutName(const QString& name);
    void setCut(TCutG* pCut);

public slots:
    virtual void accept();
    void newPoint(TPad* pad);
    void onNameChanged(const QString& name);
    void onTypeChanged(int type);

signals:
    void completed(TCutG* pCut);

private:
    void encodeRequest(const TCutG& cut);
    void insertNewPoint(double x, double y);
    void fillTableWithData(TCutG* pCut);
    void ensureLastPointDiffersFromFirst();
    void ensureLastPointMatchesFirst();
    void appendPointToCut(double x, double y);


private:
    Ui::GateBuilderDialog *ui;
    QRootCanvas& m_canvas;
    HistogramBundle& m_histPkg;
    QString m_name;
    TCutG* m_pCut;
    QButtonGroup m_radioButtons;
    bool m_matchLast;
};

#endif // GATEBUILDERDIALOG_H
