
#include "GlobalSettings.h"

QSettings* GlobalSettings::m_instance = nullptr;

QString GlobalSettings::getServerHostname() const
{
  QMutexLocker lock(&m_mutex);
  return getInstance()->value("/server/hostname").toString();
}

void GlobalSettings::setServerHostname(const QString& host) 
{
  QMutexLocker lock(&m_mutex);
  return getInstance()->setValue("/server/hostname",host);
}

int GlobalSettings::getServerPort() const
{
  QMutexLocker lock(&m_mutex);
  return getInstance()->value("/server/port").toInt();
}

void GlobalSettings::setServerPort(int port) 
{
  QMutexLocker lock(&m_mutex);
  return getInstance()->setValue("/server/port",port);
}
