#include "GateEditComHandler.h"
#include <QNetworkReply>
#include <QString>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QByteArray>
#include <QEventLoop>
#include <QMessageBox>
#include <json/json.h>
#include <stdexcept>
#include <string>
#include <exception>

using namespace std;

namespace Viewer
{

GateEditComHandler::GateEditComHandler(QObject *parent) :
    QObject(parent),
    m_pNAM(new QNetworkAccessManager)
{

}


void GateEditComHandler::makeRequest(const QUrl &req)
{

    // this "should" be very fast so allow it to block
    QEventLoop evtlp;
    connect(m_pNAM.get(), SIGNAL(finished(QNetworkReply*)),
            &evtlp, SLOT(quit()));
    auto pReply = m_pNAM->get(QNetworkRequest(req));
    evtlp.exec();

    processReply(pReply);
}


void GateEditComHandler::processReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        processSuccess(reply);
    } else {
        processFailure(reply);
    }

//    delete reply;

}


void GateEditComHandler::processSuccess(QNetworkReply *reply)
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

void GateEditComHandler::processFailure(QNetworkReply *reply)
{
    QString errMsg = reply->errorString();

    QMessageBox::warning(nullptr, "Communication Error", errMsg);
//    emit error (reply->error(), errMsg);
}

} // end of namespace
