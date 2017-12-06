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

MyCutG::MyCutG(const char* name, int n)
 : TCutG(name, n)
{
  auto pSpecials = gROOT->GetListOfSpecials();
  pSpecials->Remove(pSpecials->FindObject(name));
}

  MyCutG::MyCutG(const char* name, int n, double *x, double *y) 
: TCutG(name, n, x, y) 
{
  auto pSpecials = gROOT->GetListOfSpecials();
  pSpecials->Remove(pSpecials->FindObject(name));
}

MyCutG::~MyCutG()
{
}

void MyCutG::Paint(Option_t* opt) {

  TCutG::Paint(opt);
  
  // check if this has changed... alert others if it has 
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


/////////////////////////////////////////////////////////////////////////////
//

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
GGate& GGate::operator=(const GGate& rhs)
{
    if (this != &rhs) {
        setInfo(*rhs.m_pInfo);
        setParent(rhs.parent());
    }

    return *this;
}

// we consider equality with the spectcl state... the gui state may differ
// momentarily before being synchronized.
bool GGate::operator==(const GGate& rhs)
{
  if ( m_pInfo->getType() == SpJs::BandGate ) {
      auto& info = dynamic_cast<SpJs::Band&>(*m_pInfo);
      auto& rhsinfo = dynamic_cast<SpJs::Band&>(*rhs.m_pInfo);
      return ( info == rhsinfo );
    } else {
      auto& info = dynamic_cast<SpJs::Contour&>(*m_pInfo);
      auto& rhsinfo = dynamic_cast<SpJs::Contour&>(*rhs.m_pInfo);
      return ( info == rhsinfo );
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

void GGate::onNameChanged(const QString &name)
{
    setName(name);
}

void GGate::onPointChanged(int index, double x, double y)
{}

QString GGate::getParameterX() const
{
    return QString::fromStdString(m_pInfo->getParameter0());
}

QString GGate::getParameterY() const
{
    return QString::fromStdString(m_pInfo->getParameter1());
}

std::vector<std::pair<double, double> > GGate::getPoints() const
{
    return m_pInfo->getPoints();
}

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

SpJs::GateType GGate::getType() const
{
    return m_pInfo->getType();
}

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

void GGate::appendPoint(double x, double y)
{
    int nPoints = m_pCut->GetN();
    m_pCut->Set(nPoints+1);
    m_pCut->SetPoint(nPoints, x, y);

    m_pInfo->getPoints().push_back(std::make_pair(x, y));
}

void GGate::popBackPoint()
{
    int nPoints = m_pCut->GetN();
    m_pCut->RemovePoint(nPoints-1);

    m_pInfo->getPoints().pop_back();
}

std::pair<double, double> GGate::getPoint(size_t index) const
{
  return m_pInfo->getPoint(index);
}

void GGate::draw()
{
  if (m_pCut) {
    m_pCut->Draw("lp");
  } else {
    throw runtime_error("Cannot draw gate because it is a nullptr");
  }
}

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
