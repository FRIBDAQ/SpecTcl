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

#ifndef CONTENTREQUESTHANDLER_H
#define CONTENTREQUESTHANDLER_H


#include "HistogramList.h"
#include <QThread>
#include <QMutex>
#include <QList>
#include <QtNetwork>
#include <QWaitCondition>
#include <memory>

class QNetworkReply;
class QUrl;
class QString;
class HistogramBundle;

class ContentRequestHandler : public QThread
{
    Q_OBJECT

public:
    explicit ContentRequestHandler(QObject *parent = 0);
    virtual ~ContentRequestHandler();

    void get (const QUrl& url);
    void run();


public slots:
    void updateRequest();

signals:
    void parsingComplete(HistogramBundle* gHist);
    void error(int code, const QString& message);


private:
    void processReply(const std::unique_ptr<QNetworkReply>& reply);
    std::unique_ptr<QNetworkReply> doGet(const QNetworkRequest& req);
    QString getHistNameFromRequest(const QNetworkRequest& request);
    void completeJob();

private:
    QNetworkAccessManager m_nam;
    QList<QNetworkRequest> m_requests;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_quit;
};

#endif // ContentRequestHandler_H
