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

#ifndef GATEBUILDER_H
#define GATEBUILDER_H

#include <QDialog>
#include <QList>
#include <QPair>
#include <QString>

class TCutG;
class TPad;

class GateBuilder : public QDialog
{
    Q_OBJECT

public:
    GateBuilder(QObject* parent=0);

public slots:
    virtual void accept();

    void newPoint(TPad* pad);
    void complete();
    void nameChanged(const QString& name);

private:
    QString m_name;
    std::vector<double> m_xPoints;
    std::vector<double> m_yPoints;
};

#endif // GATEBUILDER_H
