#ifndef COMMONRESPONSEHANDLER_H
#define COMMONRESPONSEHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <memory>

class QString;
class QNetworkReply;
class QUrl;

class CommonResponseHandler : public QObject
{
    Q_OBJECT

public:
    explicit CommonResponseHandler(QObject *parent = 0);
    
    void makeRequest(const QUrl& req);

signals:
    void completed();
    void error(int code, const QString& errMsg);

public slots:
    void processReply(QNetworkReply* reply);

private:
    void processSuccess(QNetworkReply* reply);
    void processFailure(QNetworkReply* reply);

private:
    std::unique_ptr<QNetworkAccessManager> m_pNAM;
};

#endif // GATEEDITCOMHANDLER_H
