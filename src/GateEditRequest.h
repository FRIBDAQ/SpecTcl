#ifndef GATEEDITREQUEST_H
#define GATEEDITREQUEST_H

#include <QString>
#include <QUrl>

class GSlice;
class TCutG;

class GateEditRequest
{
public:
    GateEditRequest(TCutG &cut);
    GateEditRequest(const GSlice& cut);

    QUrl toUrl();

private:

    bool isBand(const TCutG& cut);

private:
    QString m_reqStr;
};

#endif // GATEEDITREQUEST_H
