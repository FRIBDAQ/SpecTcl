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

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
#include "GateListRequestHandler.h"
#include "GlobalSettings.h"

#include <QtNetwork>
#include <QMessageBox>

#include <GateCmdParser.h>
#include <json/json.h>

#include <iostream>
#include <stdexcept>

using namespace std;

namespace Viewer
{

GateListRequestHandler::GateListRequestHandler(QObject *parent) :
    QObject(parent),
    m_pReply(nullptr),
    m_pNAM(new QNetworkAccessManager),
    m_view(nullptr)
{

}

void GateListRequestHandler::get()
{
    QString host = GlobalSettings::getServerHost();
    int port     = GlobalSettings::getServerPort();
    QString urlStr("http://%1:%2/spectcl/gate/list");
    urlStr = urlStr.arg(host).arg(port);

    QUrl url(urlStr);

    m_pReply = m_pNAM->get(QNetworkRequest(url));

    connect(m_pNAM, SIGNAL(finished(QNetworkReply*)), 
            m_pReply, SLOT(finishedSlot(QNetworkReply*)));

}

void GateListRequestHandler::finishedSlot(QNetworkReply *reply)
{
    auto  error = reply->error();
    if (error == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();
        QString str = QString::fromUtf8(bytes.data(), bytes.size());

        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        try {
            // Read the response in the json
            Json::Reader reader;
            Json::Value value;
            bool ok = reader.parse(str.toAscii().constData(), value);
            if (!ok) {
                throw runtime_error ("Failed to parse json");
            }

            auto uniqueContent = SpJs::GateCmdParser().parseList(value);

            auto notUniqueContent = deuniquify_vector_contents(uniqueContent);

            emit parseCompleted(notUniqueContent);

        } catch (exception& exc) {
            QString title("Update request failure");
            QString msg("Failed to update hist because : %1");
            msg = msg.arg(QString(exc.what()));
            QMessageBox::warning(0,title,msg);
        }
    } else {

        emit parseCompleted(vector<SpJs::GateInfo*>());
    }
}

// Steal the pointers of the argument
vector<SpJs::GateInfo*> 
GateListRequestHandler::deuniquify_vector_contents(vector<unique_ptr<SpJs::GateInfo> >& unq_content)
{
  vector<SpJs::GateInfo*> retList;

  retList.reserve(unq_content.size());

  auto it = unq_content.begin();
  auto it_end = unq_content.end();
  while (it != it_end) {
    
    retList.push_back(it->release());

    ++it;
  }

  return retList;
}


} // end of namespace
