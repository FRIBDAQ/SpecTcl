#include "ListRequestHandler.h"
#include <QtNetwork>
#include <json/json.h>
#include <JsonParser.h>
#include <stdexcept>
#include <QMessageBox>
#include <GlobalSettings.h>

ListRequestHandler::ListRequestHandler(QObject *parent) :
    QObject(parent),
    m_pReply(nullptr),
    m_pNAM(new QNetworkAccessManager),
    m_view(nullptr)
{

    connect(m_pNAM, SIGNAL(finished(QNetworkReply*)), this,SLOT(finishedSlot(QNetworkReply*)));
}

void ListRequestHandler::get()
{
    QString host = GlobalSettings::getInstance()->value("/server/hostname").toString();
    int port = GlobalSettings::getInstance()->value("/server/port").toInt();
    QString urlStr("http://%1:%2/spectcl/spectrum/list");
    urlStr = urlStr.arg(host).arg(port);

    QUrl url(urlStr);
    m_pReply = m_pNAM->get(QNetworkRequest(url));
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

        } catch (std::exception& exc) {
            QString title("Update request failure");
            QString msg("Failed to update hist because : %1");
            msg = msg.arg(QString(exc.what()));
            QMessageBox::warning(0,title,msg);
        }
    } else {

        emit parseCompleted(std::vector<SpJs::HistInfo>());
    }
}
