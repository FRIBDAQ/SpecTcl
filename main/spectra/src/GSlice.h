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

#ifndef GSLICE_H
#define GSLICE_H

#include <QObject>
#include <QString>
#include <QMetaType>

#include <iosfwd>

class TFrame;

namespace SpJs 
{
  class Slice;
}

namespace Viewer
{

class QTLine;
class QRootCanvas;

/*! Graphical Slice
 *
 * This bundles together two lines and the SpecTcl information
 * for a slice. The lines are both QTLines. The graphical slice is
 * associated with a single spectcl parameter.
 *
 * The two lines that this manages are adjusted regularly so that they are
 * always vertical and they always span the height of the TFrame.
 *
 * \todo Insert a SpJs::GateInfo into this.
 */
class GSlice : public QObject
{

    // documentation can be found in the source file or in published doxygen docs.
public:
  enum DataSource { SpecTcl, GUI };

    Q_OBJECT
public:

    explicit GSlice(const SpJs::Slice& info);

    explicit GSlice(QObject *parent = 0,
                    const QString& name = QString(),
                    const QString& param = QString(),
                    double xLow = 0,
                    double xHigh = 0,
                    QRootCanvas* pCanvas=nullptr);

    virtual ~GSlice();

    GSlice& operator=(const GSlice& rhs);
    
    bool operator==(const GSlice& rhs);

    bool operator!=(const GSlice& rhs) {
      return ! (this->operator==(rhs));
    }

    // Get values of low and high 
    double getXLow() const;
    double getXHigh() const;

    // Get the graphical lines
    const QTLine* getXLowLine() const { return m_pLow; }
    const QTLine* getXHighLine() const { return m_pHigh; }

    QString getName() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    QString getParameter() const { return m_parameter; }
    void setParameter(const QString& param) { m_parameter = param; }

public slots:
    //! Draw 
    void draw(QRootCanvas* canvas);
    void draw();

    //! Update the lines
    void setXLow(double x);
    void setXHigh(double x);

    //! Slot for resizing
    void frameChanged();
    void frameChanged(TFrame* frame);

    void nameChanged(const QString& name);

    void setEditable(bool enable);
    bool isEditable() const;

private:
    QString m_name;
    QTLine* m_pLow;
    QTLine* m_pHigh;
    QRootCanvas* m_pCanvas;
    QString m_parameter;
};

} // end of namespace

extern std::ostream& operator<<(std::ostream& stream, const Viewer::GSlice& line);

#endif // GSLICE_H
