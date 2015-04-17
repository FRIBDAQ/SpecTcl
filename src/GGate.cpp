#include "GGate.h"

#include <QString>

#include <GateInfo.h>

#include <TCutG.h>

#include <stdexcept>
#include <iostream>

using namespace std;

GGate::GGate( const SpJs::GateInfo2D& info, QObject* parent)
    :
      m_info(),
      m_pCut(new MyCutG("__empty__", 1)),
      QObject(parent)
{
    // setting the info also resets the cut
    setInfo(info);
}

GGate::~GGate()
{
  cout << "GGate::~GGate() @ " << (void*)this << endl;
}
GGate& GGate::operator=(const GGate& rhs)
{
    if (this != &rhs) {
        setInfo(*rhs.m_info);
//        int n = rhs.m_pCut->GetN();
//        m_pCut.reset(new MyCutG("", n, rhs.m_pCut->GetX(), rhs.m_pCut->GetY()));

        setParent(rhs.parent());
    }

    return *this;
}

QString GGate::getName() const
{
    return QString::fromStdString(m_info->getName());
}

void GGate::setName(const QString& name) {
    return m_info->setName(name.toStdString());
}

void GGate::onNameChanged(const QString &name)
{
    setName(name);
}

void GGate::onPointChanged(int index, double x, double y)
{}

QString GGate::getParameterX() const
{
    return QString::fromStdString(m_info->getParameter0());
}

QString GGate::getParameterY() const
{
    return QString::fromStdString(m_info->getParameter1());
}

std::vector<std::pair<double, double> > GGate::getPoints() const
{
    return m_info->getPoints();
}

void GGate::setInfo(const SpJs::GateInfo2D &info)
{
    // copy the actual gate,
    // this deletes the previous gate and stores the copy
    m_info.reset(dynamic_cast<SpJs::GateInfo2D*>(info.clone().release()));

    m_pCut->SetName(m_info->getName().c_str());

    auto points = m_info->getPoints();
    size_t nPoints = points.size();

    // resize current cut
    m_pCut->Set(nPoints);

    // fill the entries of the grphical cut
    for (size_t i=0; i<nPoints; ++i) {
        auto& point = points.at(i);
        m_pCut->SetPoint(i, point.first, point.second);
    }
}

SpJs::GateType GGate::getType() const
{
    return m_info->getType();
}

void GGate::setType(SpJs::GateType type)
{
    // we are already the proper type
    if (m_info->getType()==type) {
        return;
    }

    // we are now going to change the type by swapping out
    // the SpJs::GateInfo2D object we own
    if (type == SpJs::BandGate) {
        SpJs::Band band(*m_info);
        setInfo(band);
    } else if (type == SpJs::ContourGate){
        SpJs::Contour contour(*m_info);
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

    m_info->getPoints().push_back(std::make_pair(x, y));

    auto points = m_info->getPoints();
    for (auto point : points) {
        cout << point.first << "\t" << point.second << endl;
    }

}

void GGate::popBackPoint()
{
    int nPoints = m_pCut->GetN();
    m_pCut->RemovePoint(nPoints-1);

    m_info->getPoints().pop_back();
}

void GGate::draw()
{
  if (m_pCut) {
    m_pCut->Draw("same");
  } else {
    throw runtime_error("Cannot draw gate because it is a nullptr");
  }
}

void GGate::synchronize(GGate::DataSource targ)
{
    if (targ == SpecTcl) {
        auto points = m_info->getPoints();
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

        m_info->setPoints(points);
    }
}
