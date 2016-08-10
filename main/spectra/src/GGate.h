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


/**! Make a TCutG un-special in the eyes of ROOT
 *
 *  ROOT tries to own TCutG objects and will automatically
 *  delete them from under our feet. This simple derived 
 *  class makes sure to remove the TCutG from the list of
 *  specials immediately after they have been created.
 *
 */ 
class MyCutG : public QObject, public TCutG
{
  Q_OBJECT 

  public:

  //! \brief Construct from name and number of point
  MyCutG(const char* name, int n);

  //! \brief Construct from name and number of points and data
  MyCutG(const char* name, int n, double *x, double *y);

  //! \brief Destructor
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

/*! A Graphical Gate
 *
 *  This is a class that bundles together a graphical entity (i.e. MyCutG) and 
 *  the gate information from SpecTcl. It wraps these things in to look like 
 *  any other QObject.
 *
 */
class GGate : public QObject
{

    Q_OBJECT

public:
    enum DataSource { SpecTcl, GUI };

public:
    /*! \brief Constructor
     *
     * Creates a new SpecTcl gate information.
     *
     * \param name    name of gate (\todo make this parameter go away)
     * \param info    spectcl gate information
     * \param parent  parent object that would own this.
     */
    explicit GGate(const SpJs::GateInfo2D& info,
                   QObject* parent = nullptr);

    virtual ~GGate();
    
    /*! \brief Assignment operator
     *
     * Performs a deep copy of the object. The fact that this is a QObject 
     * means that we cannot have a copy constructor but we can certainly 
     * create the same effect by creating a unique object whose state will
     * be copied.
     *
     * \param rhs   object whose state will be copied
     *
     * \returns reference to this.
     */
    GGate& operator=(const GGate& rhs);

    bool operator==(const GGate& rhs);
    bool operator!=(const GGate& rhs)
    {
      return !(this->operator==(rhs));
    }

    // Get/Set name
    QString getName() const;
    void setName(const QString& name);


    /*! \brief Get the name of the parameter for x-axis */
    QString getParameterX() const;


    /*! \brief Get the name of the parameter for y-axis */
    QString getParameterY() const;


    /*! \brief Retrieve list of point defining cut 
     *
     * \returns list of paired points. {{x0,y0}, {x1,y1}, {x2,y2}, ...}
     */
    std::vector<std::pair<double, double> > getPoints() const;

    /*!
     * \brief Retrieve a point
     * \return the x, y value of the requested point (x, y)
     *
     * \throws std::out_of_range when index is invalid
     */
    std::pair<double, double> getPoint(size_t index) const;

    //
    void setPoints(const std::vector<std::pair<double, double> >& points);

    bool isEditable() const {
      return m_pCut->GetEditable();
    }

    void setEditable(bool editable) {
      m_pCut->SetEditable(editable);
    }

    /*! \brief Sets the state according to new gate information
     *
     * This is basically the same thing as assignment. 
     *
     * \param info  gate informaton from SpecTcl
     */
    void setInfo(const SpJs::GateInfo2D& info);



    /*! \brief Appends a point to graphical entity and gate info */
    void appendPoint(double x, double y);



    /*! \brief Removes last point from graphical entity and gate info */
    void popBackPoint();


    void setPoint(int index, double x, double y);

    MyCutG* getGraphicObject() { std::cout << "m_pCut.get()=" << (void*)m_pCut.get() << std::endl; return dynamic_cast<MyCutG*>(m_pCut.get()); }

    /*! \brief Draw on the current TPad */
    void draw();



    /*! \brief Retrieves the type of gate.
     *
     *  \retval either SpJs::BandGate or SpJs::ContourGate
     */
    SpJs::GateType getType() const;




    /*! \brief Changes the type of the gate
     *
     *  This actually changes the type of the gate information.
     *
     *  \param type   either SpJs::BandGate or SpJs::ContourGate
     */
    void setType(SpJs::GateType type);


    void synchronize(DataSource targ);


    void setLineStyle(int style);
    void setLineColor(int color);
    void setLineWidth(int width);

public slots:
   /*! */
    void onPointChanged(int index, double x, double y);

    /*! \brief Updates the name */
    void onNameChanged(const QString& name);



    /// Member data 
private:
    std::unique_ptr<TCutG> m_pCut;
    std::unique_ptr<SpJs::GateInfo2D> m_pInfo;

};

} // end of namespace

#endif // GGATE_H
