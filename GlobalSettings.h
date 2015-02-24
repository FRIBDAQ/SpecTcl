#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QSettings>

// a singleton class to hold the settings
class GlobalSettings {
private:
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
};


#endif // GLOBALSETTINGS_H
