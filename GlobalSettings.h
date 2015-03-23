#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QSettings>
#include <QMutex>

/**! Singleton class to hold the settings
  *  
  *  This is a convenience wrapper around a QSettings object that
  */
class GlobalSettings {
private:
    QMutex m_mutex;
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

    QString getServerHost() const;
    void setServerHost(const QString& host);

    int getServerPort() const;
    void setServerPort(int port);
};


#endif // GLOBALSETTINGS_H
