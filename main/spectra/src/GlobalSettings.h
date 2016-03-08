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

#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include "GlobalEnum.h"
#include <QSettings>
#include <QMutex>

namespace Viewer
{

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
            m_instance = new QSettings("Spectra","NSCL");
        }

        return m_instance;
    }

    static QString getServerHost();
    static void setServerHost(const QString& host);

    static int getServerPort();
    static void setServerPort(int port);

    static QString getServer();

    static QList<QString> getAxisInfo(Vwr::Axis axis);


    static void setPollInterval(int milliseconds);
    static int getPollInterval();

    static void setSessionMode(int typeId);
    static int getSessionMode();

    static void setSharedMemoryKey(const QString& hexKey);
    static QString getSharedMemoryKey();
};

} // end of namespace

#endif // GLOBALSETTINGS_H
