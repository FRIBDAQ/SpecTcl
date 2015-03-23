#ifndef CONTENTREQUESTHANDLER_H
#define CONTENTREQUESTHANDLER_H


#include "HistogramList.h"
#include <QThread>
#include <QMutex>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QWaitCondition>
#include <memory>

class QNetworkReply;
class QUrl;
class QString;

class ContentRequestHandler : public QThread
{
    Q_OBJECT

public:
    explicit ContentRequestHandler(QObject *parent = 0);
    virtual ~ContentRequestHandler();

    void get (const QUrl& url);
    void run();


public slots:
    void updateRequest();


signals:
    void parsingComplete(const GuardedHist* gHist);
    void error(int code, const QString& message);


private:
    void processReply(const std::unique_ptr<QNetworkReply>& reply);
    std::unique_ptr<QNetworkReply> doGet(const QNetworkRequest& req);
    QString getHistNameFromRequest(const QNetworkRequest& request);
    void completeJob();

private:
    QNetworkAccessManager m_nam;
    QList<QNetworkRequest> m_requests;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_quit;
};

#endif // ContentRequestHandler_H
