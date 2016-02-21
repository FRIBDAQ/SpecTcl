//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
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

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";

#include "GlobalSettings.h"
#include <stdexcept>

namespace Viewer
{

QSettings* GlobalSettings::m_instance = nullptr;
QMutex GlobalSettings::m_mutex;

QString GlobalSettings::getServerHost()
{
  QMutexLocker lock(&m_mutex);
  return getInstance()->value("/server/hostname").toString();
}

void GlobalSettings::setServerHost(const QString& host) 
{
  QMutexLocker lock(&m_mutex);
  return getInstance()->setValue("/server/hostname",host);
}

int GlobalSettings::getServerPort()
{
  QMutexLocker lock(&m_mutex);
  return getInstance()->value("/server/port").toInt();
}

void GlobalSettings::setServerPort(int port) 
{
  QMutexLocker lock(&m_mutex);
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

  QMutexLocker lock(&m_mutex);
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

} // end of namespace
