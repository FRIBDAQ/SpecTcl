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

namespace Viewer
{

class HistogramBundle;
class HistogramList;

/*!
 * \brief The ContentRequestHandler class
 *
 * This class is responsible for handling spectrum content requests
 * over the REST server. It is used by SpecTclRESTInterface only. It
 * is a thread that serially processes a queue of requests. Each call
 * to get() adds a request to the queue.
 */
class ContentRequestHandler : public QThread
{
    Q_OBJECT


    // documentation is in the source code that can be viewed
    // in the doxygen output.

public:
    explicit ContentRequestHandler(HistogramList* pHistList, QObject *parent = 0);
    virtual ~ContentRequestHandler();


    void get (const QUrl& url);

    void run();


signals:
    void parsingComplete(HistogramBundle* gHist);
    void error(int code, const QString& message);

private:

    void  processReply(const std::unique_ptr<QNetworkReply>& reply);
    std::unique_ptr<QNetworkReply> doGet(const QNetworkRequest& req);
    QString getHistNameFromRequest(const QNetworkRequest& request);
    void completeJob();

private:
    HistogramList* m_pHistList;
    QNetworkAccessManager m_nam;
    QList<QNetworkRequest> m_requests;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_quit;
};


} // end of namespace

#endif // ContentRequestHandler_H
