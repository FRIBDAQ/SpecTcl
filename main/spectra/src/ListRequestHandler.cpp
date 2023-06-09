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

#include "ListRequestHandler.h"
#include "GlobalSettings.h"

#include <QtNetwork>
#include <QMessageBox>

#include <JsonParser.h>
#include <json/json.h>

#include <stdexcept>
#include <iostream>

namespace Viewer
{

ListRequestHandler::ListRequestHandler(QObject *parent) :
    QObject(parent),
    m_pReply(nullptr),
    m_pNAM(new QNetworkAccessManager(this)),
    m_pTimer(new QTimer(this))
{
    connect(m_pNAM, SIGNAL(finished(QNetworkReply*)), 
            this, SLOT(finishedSlot(QNetworkReply*)));

    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

void ListRequestHandler::get()
{
    QString host = GlobalSettings::getServerHost();
    int port     = GlobalSettings::getServerPort();
    QString urlStr("http://%1:%2/spectcl/spectrum/list");
    urlStr = urlStr.arg(host).arg(port);

    QUrl url(urlStr);
    m_pReply = m_pNAM->get(QNetworkRequest(url));

    connect(m_pReply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(onDownloadProgress(qint64,qint64)));
}

void ListRequestHandler::finishedSlot(QNetworkReply *reply)
{
    auto  error = reply->error();
    if (error == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();
        QString str = QString::fromUtf8(bytes.data(), bytes.size());

        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // some debugging stuff
        QFile file("out.txt");
        if (!file.open(QIODevice::WriteOnly|QIODevice::Text)) {
            throw 1;
        }
        file.write(str.toAscii());
        file.close();

        try {
            // Read the response in the json
            Json::Reader reader;
            Json::Value value;
            bool ok = reader.parse(str.toAscii().constData(),value);
            if (!ok) {
                throw std::runtime_error ("Failed to parse json");
            }

            auto content = SpJs::JsonParser().parseListCmd(value);

            emit parseCompleted(content);
            m_pTimer->stop();

        } catch (std::exception& exc) {
            QString title("Update request failure");
            QString msg("Failed to update list of histograms because : %1");
            msg = msg.arg(QString(exc.what()));
            QMessageBox::warning(0,title,msg);
        }
    } else {
//        std::cout << "Reply indicated error : " << error << std::endl;
    }
}

void ListRequestHandler::onDownloadProgress(qint64, qint64)
{
  m_pTimer->stop();
  m_pTimer->start(5000);
}

void ListRequestHandler::onTimeout()
{
    std::cout << "timeout " << std::endl;
  emit parseCompleted(std::vector<SpJs::HistInfo>());
}

} // end of namespace
