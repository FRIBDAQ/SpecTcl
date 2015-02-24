#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QUrl;
class QString;

class RequestHandler : public QObject
{
    Q_OBJECT

public:
    explicit RequestHandler(QObject *parent = 0);

    void get (const QUrl& url);

public slots:
    void finishedSlot(QNetworkReply*);
    void updateRequest();

private:
    QString getHistNameFromRequest(const QNetworkRequest& request);

private:
    QNetworkAccessManager* m_nam;
    QNetworkReply* m_reply;

};

#endif // RequestHandler_H
