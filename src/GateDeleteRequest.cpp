#include "GateDeleteRequest.h"

#include "GlobalSettings.h"
#include "GSlice.h"
#include "GGate.h"
#include <TCutG.h>

#include <iostream>

GateDeleteRequest::GateDeleteRequest(const QString& name) 
 : m_reqStr()
{
    QString server = GlobalSettings::getServer();

    m_reqStr = server + "/spectcl/gate/delete?name=";
    m_reqStr += name;
}

GateDeleteRequest::GateDeleteRequest(const GGate& cut)
    : m_reqStr()
{
    QString server = GlobalSettings::getServer();

    m_reqStr = server + "/spectcl/gate/delete";
    m_reqStr += QString("?name=") + cut.getName();
}

GateDeleteRequest::GateDeleteRequest(const GSlice &slice)
    : m_reqStr()
{
    QString server = GlobalSettings::getServer();

    m_reqStr = server + "/spectcl/gate/delete";
    m_reqStr += QString("?name=") + slice.getName();
}

QUrl GateDeleteRequest::toUrl()
{
    return QUrl(m_reqStr);
}
