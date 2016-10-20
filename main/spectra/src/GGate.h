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

#ifndef GGATE_H
#define GGATE_H

#include <QObject>

#include <GateInfo.h>

#include <memory>
#include <vector>
#include <utility>

#include <TROOT.h>
#include <TCutG.h>
#include <iostream>

namespace Viewer
{


/*! \brief Make a TCutG un-special in the eyes of ROOT
 *
 *  ROOT tries to own TCutG objects and will automatically
 *  delete them from under our feet. This simple derived 
 *  class makes sure to remove the TCutG from the list of
 *  specials immediately after they have been created.
 *
 * There is also an inheritnace of QObject that is used to
 * make the object signal/slot aware. The MyCutG object can
 * emit the modified() signal anytime that it detects that its
 * points were modified since the last draw.
 */ 
class MyCutG : public QObject, public TCutG
{
  Q_OBJECT 

    // documentation can be found in source file or in published doxygen docs
  public:

  MyCutG(const char* name, int n);

  MyCutG(const char* name, int n, double *x, double *y);

  virtual ~MyCutG();

  virtual void Paint(Option_t* opt = "");

signals:
  void modified(std::vector<std::pair<double, double> > points);

  private:
  std::vector<std::pair<double, double> > marshallData(const double* x, 
                                                       const double* y, 
                                                       size_t n);

  private:
  std::vector<double> m_pXValues;
  std::vector<double> m_pYValues;
};


////////////////////////////////////////////////////////////////////////////


/*! \brief A Graphical Gate
 *
 *  This is a class that bundles together a graphical entity (i.e. MyCutG) and 
 *  the gate information from SpecTcl. It wraps these things in to look like 
 *  any other QObject.
 *
 */
class GGate : public QObject
{

    Q_OBJECT

    // documentation can be found in source file or in published doxygen docs
public:
    enum DataSource { SpecTcl, GUI };

public:
    explicit GGate(const SpJs::GateInfo2D& info,
                   QObject* parent = nullptr);

    virtual ~GGate();
    
    GGate& operator=(const GGate& rhs);

    bool operator==(const GGate& rhs);
    bool operator!=(const GGate& rhs)
    {
      return !(this->operator==(rhs));
    }

    QString getName() const;
    void setName(const QString& name);

    QString getParameterX() const;
    QString getParameterY() const;


    std::vector<std::pair<double, double> > getPoints() const;

    std::pair<double, double> getPoint(size_t index) const;

    //
    void setPoints(const std::vector<std::pair<double, double> >& points);

    bool isEditable() const {
      return m_pCut->GetEditable();
    }

    void setEditable(bool editable) {
      m_pCut->SetEditable(editable);
    }

    void setInfo(const SpJs::GateInfo2D& info);

    void appendPoint(double x, double y);

    void popBackPoint();


    void setPoint(int index, double x, double y);

    MyCutG* getGraphicObject() { return dynamic_cast<MyCutG*>(m_pCut.get()); }

    void draw();

    SpJs::GateType getType() const;


    void setType(SpJs::GateType type);

    void synchronize(DataSource targ);


    void setLineStyle(int style);
    void setLineColor(int color);
    void setLineWidth(int width);

public slots:
    void onPointChanged(int index, double x, double y);

    void onNameChanged(const QString& name);



    /// Member data 
private:
    std::unique_ptr<TCutG> m_pCut;
    std::unique_ptr<SpJs::GateInfo2D> m_pInfo;

};

} // end of namespace

#endif // GGATE_H
