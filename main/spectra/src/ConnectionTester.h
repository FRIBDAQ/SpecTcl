#ifndef CONNECTIONTESTER_H
#define CONNECTIONTESTER_H

#include <QObject>
#include <QUrl>
#include <QNetworkReply>
#include <QTimer>

#include <memory>

namespace Viewer
{

/*!
 * \brief The ConnectionTester class
 *
 * This class is a convenience class that will check for connectivity. It
 * attempts to make some simple request of the REST server and then determines
 * whether it is connected based on the response.
 */
class ConnectionTester : public QObject
{
    Q_OBJECT

public:
    enum Result {INDETERMINATE, SUCCESS, FAILURE};

public:
    explicit ConnectionTester(const QUrl& url, QObject *parent = 0);
    
    void test();

    Result getResult() { return m_result; }

signals:
    void finished();
    
public slots:
    void onDownloadProgess(qint64,qint64);
    void onFinished();
    void onTimeout();

    static Result acceptingConnections(const QUrl &url);

private:
    QUrl                           m_url;
    QNetworkAccessManager          m_nam;
    std::unique_ptr<QNetworkReply> m_pReply;
    QTimer*                        m_pTimer;
    Result                         m_result;

};


} // end of namespace

#endif // CONNECTIONTESTER_H
