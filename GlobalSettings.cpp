
#include "GlobalSettings.h"
#include <stdexcept>

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
