#include "RequestHandler.h"
#include <QtNetwork>
#include <QString>
#include "GlobalSettings.h"
#include <json/json.h>
#include <HistFiller.h>
#include <JsonParser.h>
#include <QMessageBox>
#include "HistogramList.h"
#include <stdexcept>

RequestHandler::RequestHandler(QObject *parent) :
    QObject(parent),
    m_nam(new QNetworkAccessManager(this)),
    m_reply(nullptr)
{
//    QString proxyHost = "webproxy.nscl.msu.edu";
//    int proxyPort = 3128;
//    QNetworkProxy proxy(QNetworkProxy::HttpCachingProxy,proxyHost, proxyPort);
//    m_nam->setProxy(proxy);

    connect(m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));
}

void RequestHandler::get(const QUrl& url)
{
    m_nam->get(QNetworkRequest(url));
}

void RequestHandler::finishedSlot(QNetworkReply* reply)
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

            // parse the content... this will throw if the json
            // specifies a status other than "ok"
            auto content = SpJs::JsonParser().parseContentCmd(value);

            // get the name of the hist and update it if it exists.
            QString name = getHistNameFromRequest(reply->request());
            TH1* pHist = HistogramList::getHist(name);
            if (pHist) {
                SpJs::HistFiller()(*pHist, content);
            }
        } catch (std::exception& exc) {
            QString title("Update request failure");
            QString msg("Failed to update hist because : %1");
            msg = msg.arg(QString(exc.what()));
            QMessageBox::warning(0,title,msg);
        }
    } else {
        QString msg("Failed network request : %1");
        msg = msg.arg(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
        QMessageBox::warning(0,"Network error",msg);
    }



}


void RequestHandler::updateRequest()
{
    qDebug() << "updateRequest" << endl;
    QString host = GlobalSettings::getInstance()->value("/server/hostname").toString();
    int port = GlobalSettings::getInstance()->value("/server/port").toInt();

    QString reqStrTemplate("http://%1:%2/spectcl/spectrum/contents?name=");
    reqStrTemplate = reqStrTemplate.arg(host).arg(port);

    //  all of the
    auto names = HistogramList::histNames();
    auto iter = names.begin();
    auto end  = names.end();
    while (iter!=end) {

        QString request = (reqStrTemplate+(*iter));
        qDebug() << request << endl;
        get(request);

        ++iter;
    }

};

QString RequestHandler::getHistNameFromRequest(const QNetworkRequest &request)
{
    QUrl url = request.url();
    QString name = url.queryItemValue("name");
    return name;
}
