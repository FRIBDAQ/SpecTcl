
#include "GlobalSettings.h"

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
