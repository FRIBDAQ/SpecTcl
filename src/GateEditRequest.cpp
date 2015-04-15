#include "GateEditRequest.h"

#include "GlobalSettings.h"
#include "GSlice.h"
#include "GGate.h"
#include <TCutG.h>

#include <iostream>

GateEditRequest::GateEditRequest(const GGate& cut)
    : m_reqStr()
{
    QString server = GlobalSettings::getServer();

    m_reqStr = server + "/spectcl/gate/edit";
    m_reqStr += QString("?name=") + cut.getName();

    if (isBand(cut)) {
      m_reqStr += QString("&type=b");
    } else {
      m_reqStr += QString("&type=c");
    }
    m_reqStr += QString("&xparameter=") + cut.getParameterX();
    m_reqStr += QString("&yparameter=") + cut.getParameterY();

    auto points = cut.getPoints();

    size_t nPoints = points.size();
    for (size_t i=0; i<nPoints; ++i) {
        QString index = QString::number(i);
        QString x = QString::number(points.at(i).first);
        QString y = QString::number(points.at(i).second);
        m_reqStr += QString("&xcoord(%1)=%2").arg(index).arg(x);
        m_reqStr += QString("&ycoord(%1)=%2").arg(index).arg(y);
    }
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

bool GateEditRequest::isBand(const GGate& cut)
{
    return (cut.getType() == SpJs::BandGate );
}
