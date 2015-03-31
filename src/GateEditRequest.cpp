#include "GateEditRequest.h"

#include "GlobalSettings.h"
#include "GSlice.h"
#include <TCutG.h>

#include <iostream>

GateEditRequest::GateEditRequest(TCutG& cut)
    : m_reqStr()
{
}

GateEditRequest::GateEditRequest(const GSlice &slice)
    : m_reqStr()
{
    QString server = GlobalSettings::getServer();

    QString m_reqStr = server + "/spectcl/gate/edit";
    m_reqStr += QString("?name=") + slice.getName();
    m_reqStr += QString("&type=s&low=") + QString::number(slice.getXLow());
    m_reqStr += QString("&high=") + QString::number(slice.getXHigh());
    m_reqStr += QString("&parameter=") + slice.getParameter();
    std::cout << m_reqStr.toStdString() << std::endl;

}

QUrl GateEditRequest::toUrl()
{
    return QUrl(m_reqStr);
}
