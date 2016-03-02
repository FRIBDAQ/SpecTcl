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

#ifndef CONNECTIONTESTER_H
#define CONNECTIONTESTER_H

#include <QObject>
#include <QUrl>
#include <QNetworkReply>
#include <QTimer>

#include <memory>

namespace Viewer
{

/*!
 * \brief The ConnectionTester class
 *
 * This class is a convenience class that will check for connectivity. It
 * attempts to make some simple request of the REST server and then determines
 * whether it is connected based on the response.
 */
class ConnectionTester : public QObject
{
    Q_OBJECT

public:
    enum Result {INDETERMINATE, SUCCESS, FAILURE};

public:
    explicit ConnectionTester(const QUrl& url, QObject *parent = 0);
    
    void test();

    Result getResult() { return m_result; }

signals:
    void finished();
    
public slots:
    void onDownloadProgess(qint64,qint64);
    void onFinished();
    void onTimeout();

    static Result acceptingConnections(const QUrl &url);

private:
    QUrl                           m_url;
    QNetworkAccessManager          m_nam;
    std::unique_ptr<QNetworkReply> m_pReply;
    QTimer*                        m_pTimer;
    Result                         m_result;

};


} // end of namespace

#endif // CONNECTIONTESTER_H
