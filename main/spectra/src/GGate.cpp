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

#include "GGate.h"

#include <QString>

#include <GateInfo.h>

#include <TCutG.h>

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>

using namespace std;

namespace Viewer
{

/*! \brief Construct from name and number of points.
 *
 * This also immediately removes the TCutG from the list
 * of special objects that TROOT keeps track of.
 *
 * At the time of adding this documentation, I honestly cannot
 * recall why I chose to steal ownership from ROOT.
 *
 */
MyCutG::MyCutG(const char* name, int n)
 : TCutG(name, n)
{
  auto pSpecials = gROOT->GetListOfSpecials();
  pSpecials->Remove(pSpecials->FindObject(name));
}

/*! \brief Construct from name and number of points and data
 *
 * Immediately steals ownership from the TROOT so we can delete it
 * safely on our own time.
 */
  MyCutG::MyCutG(const char* name, int n, double *x, double *y) 
: TCutG(name, n, x, y) 
{
  auto pSpecials = gROOT->GetListOfSpecials();
  pSpecials->Remove(pSpecials->FindObject(name));
}

MyCutG::~MyCutG()
{
}

/*!
 * \brief Decorate the standard TCutG::Paint() method
 *
 * \param opt   draw options
 *
 * This is kind of a brute force implementation where we check to
 * see whether the object has been changed since the last time it was
 * updated. There may be a more intelligent way to do this that overrides
 * methods like SetPoint() to mark wehther this has been modified. That would
 * take a lot more effort to fully imkplement though and this is pretty cheap
 * to calaculate at draw time. These cuts are usually fewer than 20 points.
 */
void MyCutG::Paint(Option_t* opt) {

  TCutG::Paint(opt);
  
  // check if this has changed... alert others if it has 
  // start with the simplest comparison of size and then move
  // to actual comparisons of the points.
  if ( m_pXValues.size() != GetN() ) {

    emit modified( marshallData( GetX(), GetY(), GetN()) );

  } else if ( ! equal( m_pXValues.begin(), m_pXValues.end(),
                      GetX() ) ) {
    emit modified( marshallData( GetX(), GetY(), GetN()) );

  } else if ( ! equal( m_pYValues.begin(), m_pYValues.end(),
                       GetY() ) ) {
    emit modified( marshallData( GetX(), GetY(), GetN()) );

  }

  // update the cached values
  m_pXValues.clear();
  m_pXValues.insert( m_pXValues.begin(), GetX(), GetX() + GetN() );

  m_pYValues.clear();
  m_pYValues.insert( m_pYValues.begin(), GetY(), GetY() + GetN() );
}

/*!
 * \brief Zipper x, y arrays together
 * \param x         array of x values
 * \param y         array of y values
 * \param n         size of arrays
 * \return vector of paired points
 *
 * This works similar in function to the zip() function in python.
 * Given two lists (a0, a1, a2, ... , an) and (b0, b1, b2, ... bn),
 * this combines them to create a list of paired elements
 * ( (a0, b0), (a1,b1), (a2, b2), ... , (an, bn))
 *
 */
vector<pair<double, double> > 
MyCutG::marshallData( const double* x, const double* y, size_t n)
{
  vector<pair<double, double> > result;
  result.reserve(n);

  for (size_t index=0; index<n; ++index) {
    result.push_back( make_pair(x[index], y[index]) );
  }

  return result;
}


// end of MyCutG implementation

/////////////////////////////////////////////////////////////////////////////

/*! \brief Constructor
 *
 * Creates a new SpecTcl gate information.
 *
 * \param info    spectcl gate information
 * \param parent  parent object that would own this.
 */
GGate::GGate( const SpJs::GateInfo2D& info, QObject* parent)
    :
      m_pInfo(),
      m_pCut(new MyCutG("__empty__", 1)),
      QObject(parent)
{
    // setting the info also resets the cut
    setInfo(info);
}

GGate::~GGate()
{
}

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
GGate& GGate::operator=(const GGate& rhs)
{
    if (this != &rhs) {
        setInfo(*rhs.m_pInfo);
        setParent(rhs.parent());
    }

    return *this;
}

/*! Consider equality with the spectcl info...
 *
 * the gui state may differ  momentarily before being synchronized.
 */
bool GGate::operator==(const GGate& rhs)
{
  if ( m_pInfo->getType() == SpJs::BandGate ) {
      auto& info = dynamic_cast<SpJs::Band&>(*m_pInfo);
      return ( info == *rhs.m_pInfo );
    } else {
      auto& info = dynamic_cast<SpJs::Contour&>(*m_pInfo);
      return ( info == *rhs.m_pInfo );
    }

  // we should never ever ever get here.
  Q_ASSERT( false );

}


QString GGate::getName() const
{
    return QString::fromStdString(m_pInfo->getName());
}


void GGate::setName(const QString& name) {
    return m_pInfo->setName(name.toStdString());
}

/*! \brief Updates the name */
void GGate::onNameChanged(const QString &name)
{
    setName(name);
}


void GGate::onPointChanged(int index, double x, double y)
{}


/*! \brief Get the name of the parameter for x-axis */
QString GGate::getParameterX() const
{
    return QString::fromStdString(m_pInfo->getParameter0());
}

/*! \brief Get the name of the parameter for y-axis */
QString GGate::getParameterY() const
{
    return QString::fromStdString(m_pInfo->getParameter1());
}


/*! \brief Retrieve list of point defining cut
 *
 * \returns list of paired points. {{x0,y0}, {x1,y1}, {x2,y2}, ...}
 */
std::vector<std::pair<double, double> > GGate::getPoints() const
{
    return m_pInfo->getPoints();
}


/*! \brief Sets the state according to new gate information
 *
 * This is basically the same thing as assignment.
 *
 * \param info  gate informaton from SpecTcl
 */
void GGate::setInfo(const SpJs::GateInfo2D &info)
{
    // copy the actual gate,
    // this deletes the previous gate and stores the copy
    m_pInfo.reset(dynamic_cast<SpJs::GateInfo2D*>(info.clone().release()));

    m_pCut->SetName(m_pInfo->getName().c_str());

    auto points = m_pInfo->getPoints();
    size_t nPoints = points.size();

    // resize current cut
    m_pCut->Set(nPoints);

    // fill the entries of the grphical cut
    for (size_t i=0; i<nPoints; ++i) {
        auto& point = points.at(i);
        m_pCut->SetPoint(i, point.first, point.second);
    }

    // spectcl may not have closed the contour so we need to make sure
    // it gets closed.
    if ( info.getType() == SpJs::ContourGate ) {
      double firstX = 0;
      double firstY = 0;
      double lastX = 0;
      double lastY = 0;

      m_pCut->GetPoint(0, firstX, firstY);
      m_pCut->GetPoint(nPoints-1, lastX, lastY);

      if ( firstX != lastX || firstY != lastY ) {
        m_pCut->Set(nPoints+1);
        m_pCut->SetPoint(nPoints, firstX, firstY);
      }
    }
}


/*! \brief Retrieves the type of gate.
 *
 *  \retval either SpJs::BandGate or SpJs::ContourGate
 */
SpJs::GateType GGate::getType() const
{
    return m_pInfo->getType();
}



/*! \brief Changes the type of the gate
 *
 *  This actually changes the type of the gate information.
 *  A change between types causes manipulation of the last point defining
 *  the gate because the contours are always closed while the bands are not.
 *
 *  \param type   either SpJs::BandGate or SpJs::ContourGate
 *
 */
void GGate::setType(SpJs::GateType type)
{
    // we are already the proper type
    if (m_pInfo->getType()==type) {
        return;
    }

    // we are now going to change the type by swapping out
    // the SpJs::GateInfo2D object we own
    if (type == SpJs::BandGate) {
        SpJs::Band band(*m_pInfo);
        setInfo(band);
    } else if (type == SpJs::ContourGate){
        SpJs::Contour contour(*m_pInfo);
        setInfo(contour);
    } else {
        throw runtime_error("Cannot convert 2D gate to a slice");
    }
}


/*! \brief Appends a point to graphical entity and gate info */
void GGate::appendPoint(double x, double y)
{
    int nPoints = m_pCut->GetN();
    m_pCut->Set(nPoints+1);
    m_pCut->SetPoint(nPoints, x, y);

    m_pInfo->getPoints().push_back(std::make_pair(x, y));
}

/*! \brief Removes last point from graphical entity and gate info */
void GGate::popBackPoint()
{
    int nPoints = m_pCut->GetN();
    m_pCut->RemovePoint(nPoints-1);

    m_pInfo->getPoints().pop_back();
}

/*!
 * \brief Retrieve a point
 * \return the x, y value of the requested point (x, y)
 *
 * \throws std::out_of_range when index is invalid
 */
std::pair<double, double> GGate::getPoint(size_t index) const
{
  return m_pInfo->getPoint(index);
}

/*! \brief Draw on the current TPad with "lp" option */
void GGate::draw()
{
  if (m_pCut) {
    m_pCut->Draw("lp");
  } else {
    throw runtime_error("Cannot draw gate because it is a nullptr");
  }
}


/*!
 * \brief Synchronize state of spectcl and gui data
 *
 * \param targ either GGate::SpecTcl or GGate::GUI
 *
 * If the target is GGate::SpecTcl, the MyCutG managed by this class will
 * be updated to reflect the state of the GateInfo object. Alternatively,
 * the GateInfo object will be updated to reflect the state of the MyCutG.
 * The idea is that synchronization can go one of two directions and this
 * method can do both.
 */
void GGate::synchronize(GGate::DataSource targ)
{
    if (targ == SpecTcl) {
        auto points = m_pInfo->getPoints();
        size_t nPoints = points.size();

        // resize current cut
        m_pCut->Set(nPoints);

        // fill the entries of the grphical cut
        for (size_t i=0; i<nPoints; ++i) {
            auto& point = points.at(i);
            m_pCut->SetPoint(i, point.first, point.second);
        }
    } else {
        // resize current cut
        int nPoints = m_pCut->GetN();

        vector<pair<double, double> > points;
        points.reserve(nPoints);

        auto* pX = m_pCut->GetX();
        auto* pY = m_pCut->GetY();

        // fill the entries of the grphical cut
        for (size_t i=0; i<nPoints; ++i) {
            points.push_back(make_pair(pX[i], pY[i]));
        }

        m_pInfo->setPoints(points);
    }
}

void GGate::setPoint(int index, double x, double y)
{
  m_pCut->SetPoint(index, x, y);
}

void GGate::setLineStyle(int style)
{
    m_pCut->SetLineStyle(style);
}

void GGate::setLineColor(int color)
{
    m_pCut->SetLineColor(color);
}

void GGate::setLineWidth(int width)
{
    m_pCut->SetLineWidth(width);
}

} // end of namespace
