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

#ifndef QTLINE_H
#define QTLINE_H

#include <TLine.h>

#include <QObject>

#include <iosfwd>

namespace Viewer {
class QTLine;
}

extern std::ostream& operator<<(std::ostream& stream, const Viewer::QTLine& line);

namespace Viewer
{

class QTLine : public QObject, public TLine
{
  Q_OBJECT 

  public:
    QTLine();
    QTLine(double x1, double y1, double x2, double y2);

    QTLine& operator=(const QTLine& rhs);

    void Paint(Option_t* opts = "");

    void setEditable(bool enable);
    bool isEditable() const;
    void updateYValues();

    friend std::ostream& ::operator<<(std::ostream&, const QTLine&);
signals:
    void valuesChanged(double x1, double y1, double x2, double y2);

  private:
    bool m_editable;
    double m_lastX1;
    double m_lastY1;
    double m_lastX2;
    double m_lastY2;
};

} // end of namespace


#endif
