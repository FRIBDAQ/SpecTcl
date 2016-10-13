#ifndef VIEWER_SHMEMSIZEREQUESTHANDLER_H
#define VIEWER_SHMEMSIZEREQUESTHANDLER_H

#include <QObject>

class QByteArray;
class QNetworkReply;

namespace Viewer {


class ShMemSizeRequestHandler : public QObject
{
    Q_OBJECT
public:
    explicit ShMemSizeRequestHandler(QObject *parent = 0);

    void get();

    void processSuccess(const QByteArray& buffer);
    void processFailure(QNetworkReply& reply);

    void processReply(QNetworkReply* pReply);
};

} // namespace Viewer

#endif // VIEWER_SHMEMSIZEREQUESTHANDLER_H
