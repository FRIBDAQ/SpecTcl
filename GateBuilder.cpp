#include "GateBuilder.h"

#include <TCutG.h>
#include <TPad.h>

GateBuilder::GateBuilder(QObject* parent)
    : QDialog(parent), m_xPoints(), m_yPoints()
{
}

void GateBuilder::newPoint(TPad *pad)
{
    Q_ASSERT(pad!=nullptr);

    double x, y;
    pad->AbsPixeltoXY(pad->GetEventX(), pad->GetEventY(), x, y);

    m_xPoints.push_back(x);
    m_yPoints.push_back(y);
}

void GateBuilder::nameChanged(const QString &name)
{
    m_name = name;
}

void GateBuilder::complete()
{
    TCutG cut(m_name.toStdString().c_str(),
              m_xPoints.size(),
              m_xPoints.data(),
              m_yPoints.data());

    emit completed(cut);
}
