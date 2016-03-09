#include "ShMemSizeRequestHandler.h"
#include "GlobalSettings.h"
#include "SharedMemoryKeyParser.h"
#include <json/json.h>

#include <QByteArray>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QMessageBox>

#include <stdexcept>

namespace Viewer {

ShMemSizeRequestHandler::ShMemSizeRequestHandler(QObject *parent) :
    QObject(parent)
{
}

void ShMemSizeRequestHandler::get()
{
    QEventLoop loop;
    QNetworkAccessManager nam;

    QString request = GlobalSettings::getServer() + "/spectcl/shmem/size";
    QNetworkReply* pReply = nam.get(QNetworkRequest(QUrl(request)));

    connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    loop.exec();

    processReply(pReply);

}

void ShMemSizeRequestHandler::processReply(QNetworkReply *pReply)
{

    if (pReply->error() == QNetworkReply::NoError) {
        QByteArray buffer = pReply->readAll();

        processSuccess(buffer);

    } else {
        processFailure(*pReply);
    }
}

void ShMemSizeRequestHandler::processSuccess(const QByteArray& buffer)
{
    Json::Reader reader;
    Json::Value value;

    bool ok = reader.parse(buffer.constData(), value);

    if (!ok) {
        throw std::runtime_error ("Failure while parsing response. Malformed data.");
    }

    SpJs::SharedMemoryKeyParser parser;
    int size = ::atoi(parser.parseSize(value).c_str());
    GlobalSettings::setSharedMemorySize(size);
}

void ShMemSizeRequestHandler::processFailure(QNetworkReply &reply)
{
    QMessageBox::warning(nullptr,
                         "Invalid server response",
                         "Failed to parse the shared memory size");

}


} // namespace Viewer
