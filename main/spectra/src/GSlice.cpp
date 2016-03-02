//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
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

#include "GSlice.h"
#include "QRootCanvas.h"
#include "QTLine.h"
#include <TCanvas.h>
#include <TFrame.h>

#include <GateInfo.h>
#include <iostream>

using namespace std;

namespace Viewer
{

GSlice::GSlice(QObject *parent, const QString &name,
               const QString& param,
               double xLow, double xHigh,
               QRootCanvas* pCanvas)
    :
    QObject(parent),
    m_name(name),
    m_pLow(new QTLine(xLow, 0, xLow, 1)),
    m_pHigh(new QTLine(xHigh, 0, xHigh, 1)),
    m_pCanvas(pCanvas),
    m_parameter(param)
{
    if (m_name.isEmpty()) {
        m_name = "__slice_in_progress__";
    }
}

GSlice::GSlice(const SpJs::Slice& info)
: GSlice(nullptr, 
          QString::fromStdString(info.getName()), 
          QString::fromStdString(info.getParameter()), 
          info.getLowerLimit(), 
          info.getUpperLimit(), 
          nullptr)
{
  if (m_name.isEmpty()) {
    m_name = "__slice_in_progress__";
  }
}

GSlice::~GSlice()
{
    delete m_pLow;
    m_pLow = nullptr;

    delete m_pHigh;
    m_pHigh = nullptr;
}


GSlice& GSlice::operator=(const GSlice& rhs)
{
    if (this != &rhs) {
        *m_pLow = *rhs.getXLowLine();
        *m_pHigh = *rhs.getXHighLine();

        m_name = rhs.m_name;

        m_pCanvas = rhs.m_pCanvas;
        m_parameter = rhs.m_parameter;

    }
    return *this;
}

bool GSlice::operator ==(const GSlice& rhs)
{

  return ((m_name == rhs.m_name) && (getXLow() == rhs.getXLow())
          && (getXHigh() == rhs.getXHigh()) && (getParameter() == rhs.getParameter()));
}

void GSlice::draw(QRootCanvas *pCanvas)
{
    Q_ASSERT(pCanvas != nullptr);

    m_pCanvas = pCanvas;

    frameChanged();

    m_pLow->Draw("same");
    m_pHigh->Draw("same");

}

double GSlice::getXLow() const
{
    return m_pLow->GetX1();
}

double GSlice::getXHigh() const
{
    return m_pHigh->GetX1();
}

void GSlice::draw()
{
  std::cout << "GSlice::draw()" << std::endl;
    TFrame* pFrame = gPad->GetFrame();

    m_pLow->Draw("same");
    m_pHigh->Draw("same");

}
void GSlice::setXLow(double x)
{
    m_pLow->SetX1(x);
    m_pLow->SetX2(x);
}

void GSlice::setXHigh(double x)
{
    m_pHigh->SetX1(x);
    m_pHigh->SetX2(x);
}


void GSlice::frameChanged()
{
    Q_ASSERT(m_pCanvas != nullptr);

    auto pFrame = m_pCanvas->getCanvas()->GetFrame();

    frameChanged(pFrame);
}

void GSlice::frameChanged(TFrame* pFrame)
{
    Q_ASSERT(pFrame != nullptr);

    auto yLow  = pFrame->GetY1();
    auto yHigh = pFrame->GetY2();

    m_pLow->SetY1(yLow);
    m_pLow->SetY2(yHigh);

    m_pHigh->SetY1(yLow);
    m_pHigh->SetY2(yHigh);
}

void GSlice::nameChanged(const QString &name)
{
    m_name = name;
}

void GSlice::setEditable(bool enable) 
{
  m_pLow->setEditable(enable);
  m_pHigh->setEditable(enable);
}

bool GSlice::isEditable() const 
{
  return (m_pLow->isEditable() && m_pHigh->isEditable());
}

} // end of namespace

ostream& operator<<(ostream& stream, const Viewer::GSlice& slice)
{
  stream << "GSlice @ " << (void*) &slice;
  stream << "\nlow = " << slice.getXLow();
  stream << "\thigh = " << slice.getXHigh();

  return stream;
}
