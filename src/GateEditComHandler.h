#ifndef GATEEDITCOMHANDLER_H
#define GATEEDITCOMHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <memory>

class QString;
class QNetworkReply;
class QUrl;

namespace Viewer
{

class GateEditComHandler : public QObject
{
    Q_OBJECT

public:
    explicit GateEditComHandler(QObject *parent = 0);
    
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

} // end of namespace

#endif // GATEEDITCOMHANDLER_H

