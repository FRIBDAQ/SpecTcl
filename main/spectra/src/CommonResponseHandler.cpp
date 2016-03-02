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

#include "CommonResponseHandler.h"
#include <QNetworkReply>
#include <QString>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QByteArray>
#include <QMessageBox>
#include <json/json.h>
#include <stdexcept>
#include <string>
#include <exception>

using namespace std;

namespace Viewer
{

CommonResponseHandler::CommonResponseHandler(QObject *parent) :
    QObject(parent),
    m_pNAM(new QNetworkAccessManager)
{
    connect(m_pNAM.get(), SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));
}


void CommonResponseHandler::makeRequest(const QUrl &req)
{
    m_pNAM->get(QNetworkRequest(req));
}


void CommonResponseHandler::processReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        processSuccess(reply);
    } else {
        processFailure(reply);
    }

//    delete reply;

}


void CommonResponseHandler::processSuccess(QNetworkReply *reply)
{
    QByteArray bytes = reply->readAll();

    Json::Value value;
    Json::Reader reader;

    try {
        reader.parse(bytes.constData(), value);

        if (value["status"] != "OK") {

            auto response = value["detail"].asString();

            throw runtime_error(response);
        }

    } catch (exception exc) {
        QString msg("Failed to update hist because : %1");
        msg = msg.arg(QString(exc.what()));

        // FAIL....
 //       emit this->error(0, msg);
        QMessageBox::warning(nullptr, "JSON Parsing Error", msg);
    } catch (...) {
        rethrow_exception( current_exception() );
    }

    emit completed();
}

void CommonResponseHandler::processFailure(QNetworkReply *reply)
{
    QString errMsg = reply->errorString();

    QMessageBox::warning(nullptr, "Communication Error", errMsg);
//    emit error (reply->error(), errMsg);
}



} // end of namespace

