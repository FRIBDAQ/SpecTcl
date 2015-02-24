#include "test.h"
#include <QtNetwork>
#include <QString>

Test::Test(QObject *parent) :
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

void Test::get(const QUrl& url)
{
    m_nam->get(QNetworkRequest(url));
}

void Test::finishedSlot(QNetworkReply* reply)
{

    auto  error = reply->error();
    if (error == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();
        QString str = QString::fromUtf8(bytes.data(), bytes.size());
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << str << endl;
    } else {
        qDebug() << "Failed with error " << error << endl;
    }
}


