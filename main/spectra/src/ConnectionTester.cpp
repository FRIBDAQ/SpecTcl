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

#include "ConnectionTester.h"
#include <QtNetwork>

#include <iostream>

using namespace std;

namespace Viewer
{


ConnectionTester::ConnectionTester(const QUrl &url, QObject *parent) :
    QObject(parent),
    m_url(url),
    m_nam(),
    m_pTimer(new QTimer(this)),
    m_result(INDETERMINATE)
{
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

void ConnectionTester::test()
{
    m_result = INDETERMINATE;
    m_pReply.reset( m_nam.get( QNetworkRequest(m_url)) );

    connect(m_pReply.get(), SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(onDownloadProgess(qint64,qint64)));

    connect(m_pReply.get(), SIGNAL(finished()),
            this, SLOT(onFinished()));

}

void ConnectionTester::onDownloadProgess(qint64, qint64)
{
    m_pTimer->stop();
    m_pTimer->start(1000);
}

void ConnectionTester::onTimeout()
{
    m_result = FAILURE;
    emit finished();
}

void ConnectionTester::onFinished()
{
    m_pTimer->stop();

    if (m_pReply->error() == QNetworkReply::NoError) {
        m_result = SUCCESS;

    } else {
        m_result = FAILURE;
    }
    emit finished();
}




ConnectionTester::Result ConnectionTester::acceptingConnections(const QUrl &url)
{
    ConnectionTester tester(url, 0);

    tester.test();
    QEventLoop eLoop;
    connect(&tester, SIGNAL(finished()), &eLoop, SLOT(quit()));

    eLoop.exec();
    return tester.getResult();
}

} // end of namespace
