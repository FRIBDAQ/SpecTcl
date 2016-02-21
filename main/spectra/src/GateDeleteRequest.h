#ifndef GATEDELETEREQUEST_H
#define GATEDELETEREQUEST_H

#include <QString>
#include <QUrl>

namespace Viewer
{

class GSlice;
class GGate;

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

}  // end of namespace

#endif // GATEDELETEREQUEST_H
