#ifndef GATEEDITREQUEST_H
#define GATEEDITREQUEST_H

#include <QString>
#include <QUrl>

namespace Viewer
{

class GSlice;
class GGate;

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

} // end of namespace

#endif // GATEEDITREQUEST_H

