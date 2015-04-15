#ifndef GATEEDITREQUEST_H
#define GATEEDITREQUEST_H

#include <QString>
#include <QUrl>

class GSlice;
class GGate;
class TCutG;

class GateEditRequest
{
public:
    GateEditRequest(const GGate &cut);
    GateEditRequest(const GSlice& cut);

    QUrl toUrl();

private:

    bool isBand(const GGate& cut);

private:
    QString m_reqStr;
};

#endif // GATEEDITREQUEST_H
