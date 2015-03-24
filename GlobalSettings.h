#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include "GlobalEnum.h"
#include <QSettings>
#include <QMutex>

/**! Singleton class to hold the settings
  *  
  *  This is a convenience wrapper around a QSettings object that
  */
class GlobalSettings {
private:
    static QMutex m_mutex;
    static QSettings* m_instance;

    GlobalSettings() = default;
    GlobalSettings(const GlobalSettings& rhs) = delete;
    ~GlobalSettings() {
        delete m_instance;
    }

public:
    static QSettings* getInstance() {
        if (m_instance==nullptr) {
            m_instance = new QSettings("Viewer","NSCL");
        }

        return m_instance;
    }

    static QString getServerHost();
    static void setServerHost(const QString& host);

    static int getServerPort();
    static void setServerPort(int port);

    static QList<QString> getAxisInfo(Vwr::Axis axis);
};


#endif // GLOBALSETTINGS_H
