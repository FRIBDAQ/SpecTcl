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

#ifndef LISTREQUESTHANDLER_H
#define LISTREQUESTHANDLER_H

#include "QHistInfo.h"

#include <QObject>
#include <QList>
#include <QTimer>
#include <vector>

class QNetworkReply;
class QNetworkAccessManager;

namespace Viewer
{

class HistogramView;

class ListRequestHandler : public QObject
{
    Q_OBJECT
public:
    explicit ListRequestHandler(QObject *parent = 0);
    
    void get();

public slots:
    void finishedSlot(QNetworkReply* reply);
    void onDownloadProgress(qint64,qint64);
    void onTimeout();

signals:
    void parseCompleted(std::vector<SpJs::HistInfo> nameList);

private:
    QNetworkReply*         m_pReply;
    QNetworkAccessManager* m_pNAM;
    QTimer*                m_pTimer;
};

} // end of namespace 

#endif // LISTREQUESTHANDLER_H
