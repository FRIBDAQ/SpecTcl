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

#include "QTLine.h"

#include <TFrame.h>
#include <TPad.h>

#include <iostream>

using namespace std;

namespace Viewer
{

//
//
QTLine::QTLine() : QTLine(0,0,0,0)
{}

//
//
QTLine::QTLine(double x1, double y1, double x2, double y2)
  : QObject(),
    TLine(x1, y1, x2, y2),
    m_editable(false),
    m_lastX1(x1),
    m_lastY1(y1),
    m_lastX2(x2),
    m_lastY2(y2)
{
}

//
//
QTLine& QTLine::operator=(const QTLine& rhs) {
  if (this != &rhs) {
      m_editable = rhs.m_editable;

      m_lastX1 = rhs.m_lastX1;
      m_lastY1 = rhs.m_lastY1;
      m_lastX2 = rhs.m_lastX2;
      m_lastY2 = rhs.m_lastY2;


      TLine::operator=(rhs);
    }
  return *this;
}

//
//
void QTLine::Paint(Option_t* opt) {

  // update the y1 and y2
  updateYValues();

  if ( ! m_editable ) {
      // if you cannot edit it, then get rid of changes...
      TLine::SetX1(m_lastX1);
      TLine::SetY1(m_lastY1);
      TLine::SetX2(m_lastX2);
      TLine::SetY2(m_lastY2);
  } else {
      if (m_lastX1 != GetX1() || m_lastY1 != GetY1()
          || m_lastX2 != GetX2() || m_lastY2 != GetY2() ) {
          // value changed since last paint
          emit valuesChanged(GetX1(), GetY1(), GetX2(), GetY2() );
      }
  }

  TLine::Paint(opt);

  m_lastX1 = GetX1();
  m_lastY1 = GetY1();
  m_lastX2 = GetX2();
  m_lastY2 = GetY2();
}

//
//
void QTLine::setEditable(bool enable) {
  m_editable = enable;
  // line should show up thick and dashed if editable
  if (m_editable) {
      SetLineWidth(2);
      SetLineStyle(2);
  } else {
      SetLineWidth(1);
      SetLineStyle(1);
  }
}

//
//
bool QTLine::isEditable() const {
  return m_editable;

}

//
//
void QTLine::updateYValues()
{
  auto pFrame = gPad->GetFrame();

  SetY1( pFrame->GetY1() );
  SetY2( pFrame->GetY2() );

  m_lastY1 = GetY1();
  m_lastY2 = GetY2();
}


} // end of namespace

ostream& operator<<(ostream& stream, const Viewer::QTLine& line)
{
  stream << "QTLine @ " << (void*) &line;
  stream << " (" << line.GetX1() << ", " <<line.GetY1() << ") --> ";
  stream << "(" << line.GetX2() << ", " << line.GetY2() << ")" << endl;
  stream << "\t(" << line.fX1 << ", " << line.fY1 << ") --> ";
  stream << "(" << line.fX2 << ", " << line.fY2 << ")";

  return stream;
}
