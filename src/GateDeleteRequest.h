#ifndef GATEDELETEREQUEST_H
#define GATEDELETEREQUEST_H

#include <QString>
#include <QUrl>

class GSlice;
class GGate;
class TCutG;

class GateDeleteRequest
{
public:
    GateDeleteRequest(const QString& name);
    GateDeleteRequest(const GGate &cut);
    GateDeleteRequest(const GSlice& cut);

    QUrl toUrl();

private:
    QString m_reqStr;
};

#endif // GATEDELETEREQUEST_H
