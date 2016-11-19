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

public:
    explicit ContentRequestHandler(HistogramList* pHistList, QObject *parent = 0);
    virtual ~ContentRequestHandler();

    /*!
     * \brief Queue a request for content
     *
     * \param url  url for the request
     *
     * This is the entry point for requests. It is typically going to be called
     * from a different thread than the thread that is running the logic of the
     * run method. This method can wake the thread that is waiting.
     */
    void get (const QUrl& url);

    /*!
     * \brief The child thread logic
     *
     * This is just a while loop that will process jobs sequentially. Once the
     * queue is empty, it will wait on a condition variable until a new request
     * arrives.
     */
    void run();


signals:
    void parsingComplete(HistogramBundle* gHist);
    void error(int code, const QString& message);


private:
    /*!
     * \brief Handles a completed reply
     *
     * \param reply  the response from the server
     *
     * This is where the actual response is processed.
     */
    void processReply(const std::unique_ptr<QNetworkReply>& reply);

    /*!
     * \brief Send the REST request to the server.
     *
     * \param req   network request
     * \return the network reply object
     */
    std::unique_ptr<QNetworkReply> doGet(const QNetworkRequest& req);

    /*!
     * \brief getHistNameFromRequest
     * \param request the url of the request
     * \return the name of the histogram
     */
    QString getHistNameFromRequest(const QNetworkRequest& request);

    /*!
     * \brief completeJob
     *
     * This clears the job from the queue. If the queue is empty after this,
     * then thread waits on the condition variable.
     */
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
