#include "ShMemKeyRequestHandler.h"
#include "GlobalSettings.h"

#include "SharedMemoryKeyParser.h"

#include <QNetworkReply>
#include <QByteArray>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QEventLoop>

#include <json/json.h>

#include <stdexcept>
#include <iostream>

namespace Viewer {

ShMemKeyRequestHandler::ShMemKeyRequestHandler()
{
}


void ShMemKeyRequestHandler::get()
{
    QEventLoop loop;
    QNetworkAccessManager nam;

    QString request = GlobalSettings::getServer() + "/spectcl/shmem/key";
    std::cout << "get() " << request.toStdString() << std::endl;
    QNetworkReply* pReply = nam.get(QNetworkRequest(QUrl(request)));

    connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    loop.exec();

    std::cout << "done waiting" << std::endl;
    processReply(pReply);

}

void ShMemKeyRequestHandler::processReply(QNetworkReply *pReply)
{

    if (pReply->error() == QNetworkReply::NoError) {
        QByteArray buffer = pReply->readAll();

        processSuccess(buffer);

    } else {
        processFailure(*pReply);
    }
}

void ShMemKeyRequestHandler::processSuccess(const QByteArray& buffer)
{
    Json::Reader reader;
    Json::Value value;

    bool ok = reader.parse(buffer.constData(), value);

    if (!ok) {
        throw std::runtime_error ("Failure while parsing response. Malformed data.");
    }

    SpJs::SharedMemoryKeyParser parser;
    QString key = QString::fromStdString(parser.parseKey(value));
    GlobalSettings::setSharedMemoryKey(key);
}

void ShMemKeyRequestHandler::processFailure(QNetworkReply &reply)
{
    QMessageBox::warning(nullptr,
                         "Invalid server response",
                         "Failed to parse the shared memory key");

}

} // namespace Viewer
