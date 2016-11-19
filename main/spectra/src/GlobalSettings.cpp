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

#include "GlobalSettings.h"
#include <stdexcept>

namespace Viewer
{

QSettings* GlobalSettings::m_instance = nullptr;
QMutex GlobalSettings::m_mutex;
bool GlobalSettings::m_batchMode = false;

QString GlobalSettings::getServerHost()
{
  return getInstance()->value("/server/hostname").toString();
}

void GlobalSettings::setServerHost(const QString& host) 
{
  return getInstance()->setValue("/server/hostname",host);
}

int GlobalSettings::getServerPort()
{
  return getInstance()->value("/server/port").toInt();
}

void GlobalSettings::setServerPort(int port) 
{
  return getInstance()->setValue("/server/port",port);
}

QString GlobalSettings::getServer()
{
    return QString("http://%1:%2").arg(getServerHost()).arg(QString::number(getServerPort()));
}

QList<QString> GlobalSettings::getAxisInfo(Vwr::Axis axis)
{
  QList<QString> resp;

  QString axisName;
  if (axis == Vwr::XAxis) {
    axisName = "xaxis"; 
  } else if (axis == Vwr::YAxis) {
    axisName = "yaxis"; 
  } else {
    throw std::runtime_error("GlobalSettings::getAxisInfo() Invalid axis specifier");
  }

  QString nbins("/hist/%1/nbins");
  QString min("/hist/%1/min");
  QString max("/hist/%1/max");

  resp.push_back(getInstance()->value(nbins.arg(axisName)).toString());
  resp.push_back(getInstance()->value(min.arg(axisName)).toString());
  resp.push_back(getInstance()->value(max.arg(axisName)).toString());

  return resp;
}

int GlobalSettings::getPollInterval()
{
  return getInstance()->value("/server/pollInterval").toInt();
}

void GlobalSettings::setPollInterval(int milliseconds)
{
  return getInstance()->setValue("/server/pollInterval", milliseconds);
}

void GlobalSettings::setSessionMode(SessionType typeId)
{
    if (typeId == REMOTE) {
        getInstance()->setValue("/session/mode", "remote");
    } else {
        getInstance()->setValue("/session/mode", "local");
    }
}

GlobalSettings::SessionType GlobalSettings::getSessionMode()
{
    QString mode = getInstance()->value("/session/mode").toString();
    if (mode == "remote") {
        return REMOTE;
    } else if (mode == "local") {
        return LOCAL;
    } else {
        throw std::runtime_error("GlobalSettings::getSessionMode() Invalid mode");
    }
}

void GlobalSettings::setSharedMemoryKey(const QString& keyString)
{
    return getInstance()->setValue("/session/shmem/key", keyString);
}

QString GlobalSettings::getSharedMemoryKey()
{
    return getInstance()->value("/session/shmem/key").toString();
}

void GlobalSettings::setSharedMemorySize(int size)
{
    return getInstance()->setValue("/session/shmem/size", size);
}

QString GlobalSettings::getSharedMemorySize()
{
    return getInstance()->value("/session/shmem/size").toString();
}

bool GlobalSettings::getBatchMode()
{
    return m_batchMode;
}

void GlobalSettings::setBatchMode(bool mode)
{
    m_batchMode = mode;
}

} // end of namespace
