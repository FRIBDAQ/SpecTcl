#ifndef LISTREQUESTHANDLER_H
#define LISTREQUESTHANDLER_H

#include <QObject>
#include <QList>
#include <vector>
#include <HistInfo.h>

class QNetworkReply;
class QNetworkAccessManager;
class HistogramView;

class ListRequestHandler : public QObject
{
    Q_OBJECT
public:
    explicit ListRequestHandler(QObject *parent = 0);
    
    void get();

    void setHistogramView(HistogramView* view) {
      m_view = view;
    }

public slots:
    void finishedSlot(QNetworkReply* reply);

signals:
    void parseCompleted(std::vector<SpJs::HistInfo> nameList);

private:
    QNetworkReply*         m_pReply;
    QNetworkAccessManager* m_pNAM;
    HistogramView*         m_view;
};

#endif // LISTREQUESTHANDLER_H
