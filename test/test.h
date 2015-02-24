#ifndef TEST_H
#define TEST_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QUrl;

class Test : public QObject
{
    Q_OBJECT
public:
    explicit Test(QObject *parent = 0);

    void get (const QUrl& url);

public slots:
    void finishedSlot(QNetworkReply*);

    private:
    QNetworkAccessManager* m_nam;
    QNetworkReply* m_reply;

};

#endif // TEST_H
