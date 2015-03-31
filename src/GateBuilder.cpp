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

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
#include "GateBuilder.h"

#include <TCutG.h>
#include <TPad.h>

GateBuilder::GateBuilder(QObject* parent)
    : QDialog(parent), m_xPoints(), m_yPoints()
{
}

void GateBuilder::newPoint(TPad *pad)
{
    Q_ASSERT(pad!=nullptr);

    double x, y;
    pad->AbsPixeltoXY(pad->GetEventX(), pad->GetEventY(), x, y);

    m_xPoints.push_back(x);
    m_yPoints.push_back(y);
}

void GateBuilder::nameChanged(const QString &name)
{
    m_name = name;
}

void GateBuilder::complete()
{
    TCutG cut(m_name.toStdString().c_str(),
              m_xPoints.size(),
              m_xPoints.data(),
              m_yPoints.data());

    emit completed(cut);
}
