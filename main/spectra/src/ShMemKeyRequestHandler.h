#ifndef VIEWER_SHMEMKEYREQUESTHANDLER_H
#define VIEWER_SHMEMKEYREQUESTHANDLER_H

#include <QObject>

class QNetworkReply;

namespace Viewer {

class ShMemKeyRequestHandler : public QObject
{
    Q_OBJECT

public:
    ShMemKeyRequestHandler();

    void get();

public slots:
    void processReply(QNetworkReply* pReply);

    void processSuccess(const QByteArray& buffer);

    void processFailure(QNetworkReply& reply);
};

} // namespace Viewer

#endif // VIEWER_SHMEMKEYREQUESTHANDLER_H
