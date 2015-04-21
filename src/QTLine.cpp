
#include "QTLine.h"

#include <TFrame.h>
#include <TPad.h>

#include <ostream>

using namespace std;

QTLine::QTLine()
  : QObject(),
    TLine(),
    m_editable(false),
    m_lastX1(0),
    m_lastY1(0),
    m_lastX2(0),
    m_lastY2(0)
{
  SetVertical(true);
}

QTLine::QTLine(double x1, double y1, double x2, double y2)
  : QObject(),
    TLine(x1, y2, x2, y2),
    m_editable(false),
    m_lastX1(x1),
    m_lastY1(y1),
    m_lastX2(x2),
    m_lastY2(y2)
{}

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

void QTLine::setEditable(bool enable) {
  m_editable = enable;
}

bool QTLine::isEditable() const {
  return m_editable;

}

void QTLine::updateYValues()
{
  auto pFrame = gPad->GetFrame();

  SetY1( pFrame->GetY1() );
  SetY2( pFrame->GetY2() );

  m_lastY1 = GetY1();
  m_lastY2 = GetY2();
}


ostream& operator<<(ostream& stream, const QTLine& line)
{
  stream << "QTLine @ " << (void*) &line;
  stream << "\n  x0 = " << line.GetX1();
  stream << "\n  y0 = " << line.GetY1();
  stream << "\n  x1 = " << line.GetX2();
  stream << "\n  y1 = " << line.GetY2();
  stream << endl;

  return stream;
}
