#include "GateEditRequest.h"

#include "GlobalSettings.h"
#include "GSlice.h"
#include <TCutG.h>

#include <iostream>

GateEditRequest::GateEditRequest(TCutG& cut)
    : m_reqStr()
{
    QString server = GlobalSettings::getServer();

    m_reqStr = server + "/spectcl/gate/edit";
    m_reqStr += QString("?name=") + cut.GetName();

    if (isBand(cut)) {
      m_reqStr += QString("&type=b");
    } else {
      m_reqStr += QString("&type=c");
    }
    m_reqStr += QString("&xparameter=") + cut.getXParameter();
    m_reqStr += QString("&yparameter=") + cut.getYParameter();
    m_reqStr += QString("&xcoords={");

    int nPoints = cut.GetN();
    auto pX = cut.GetX();
    for (int i=0; i<nPoints; ++i) {
      m_reqStr += QString::number(pX[i]);
      if (i<nPoints-1) {
        m_reqStr += " ";
      }
    }
    m_reqStr += QString("}&ycoords={");
    auto pY = cut.GetX();
    for (int i=0; i<nPoints; ++i) {
      m_reqStr += QString::number(pY[i]);
      if (i<nPoints-1) {
        m_reqStr += " ";
      }
    }
    m_reqStr += QString("}");
}

GateEditRequest::GateEditRequest(const GSlice &slice)
    : m_reqStr()
{
    QString server = GlobalSettings::getServer();

    m_reqStr = server + "/spectcl/gate/edit";
    m_reqStr += QString("?name=") + slice.getName();
    m_reqStr += QString("&type=s&low=") + QString::number(slice.getXLow());
    m_reqStr += QString("&high=") + QString::number(slice.getXHigh());
    m_reqStr += QString("&parameter=") + slice.getParameter();

}

QUrl GateEditRequest::toUrl()
{
    return QUrl(m_reqStr);
}

bool GateEditRequest::isBand(const TCutG& cut)
{
  auto n = cut.GetN();
  auto pX = cut.GetX();
  auto pY = cut.GetY();

  if (n==0) {
    return true;
  } else {
    return ((pX[0] == pX[n-1]) && (pY[0] == pY[n-1]));
  }
}
