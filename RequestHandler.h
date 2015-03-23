#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H


#include "HistogramList.h"
#include <QThread>
#include <QMutex>
#include <QUrl>
#include <QWaitCondition>

class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QUrl;
class QString;

class RequestHandler : public QThread
{
    Q_OBJECT

public:
    explicit RequestHandler(QObject *parent = 0);
    virtual ~RequestHandler();

    void get (const QUrl& url);
    void run();


public slots:
    void updateRequest();


signals:
    void parsingComplete(const GuardedHist* gHist);
    void error(int code, const QString& message);


private:
    QString getHistNameFromRequest(const QNetworkRequest& request);
    QByteArray decompress(const QByteArray& compStr);

    int unCompress(QByteArray& ucBuffer, const QByteArray& cBuffer);

private:
    QUrl m_url;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_quit;
};

#endif // RequestHandler_H
