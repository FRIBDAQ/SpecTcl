#include "GSlice.h"
#include "QRootCanvas.h"
#include <TLine.h>
#include <TCanvas.h>
#include <TFrame.h>

#include <GateInfo.h>

GSlice::GSlice(QObject *parent, const QString &name,
               const QString& param,
               double xLow, double xHigh,
               QRootCanvas* pCanvas)
    :
    QObject(parent),
    m_name(name),
    m_pLow(new TLine(xLow, 0, xLow, 1)),
    m_pHigh(new TLine(xHigh, 0, xHigh, 1)),
    m_pCanvas(pCanvas),
    m_parameter(param)
{
    if (m_name.isEmpty()) {
        m_name = "__slice_in_progress__";
    }
}

GSlice::GSlice(const SpJs::Slice& info)
: GSlice(nullptr, 
          QString::fromStdString(info.getName()), 
          QString::fromStdString(info.getParameter()), 
          info.getLowerLimit(), 
          info.getUpperLimit(), 
          nullptr)
{
  if (m_name.isEmpty()) {
    m_name = "__slice_in_progress__";
  }
}

GSlice::~GSlice()
{
    delete m_pLow;
    m_pLow = nullptr;

    delete m_pHigh;
    m_pHigh = nullptr;
}


GSlice& GSlice::operator=(const GSlice& rhs)
{
    if (this != &rhs) {
        auto pTemp = new TLine(*rhs.m_pLow);
        delete m_pLow;
        m_pLow = pTemp;

        pTemp = new TLine(*rhs.m_pHigh);
        delete m_pHigh;
        m_pHigh = pTemp;

        m_name = rhs.m_name;

        m_pCanvas = rhs.m_pCanvas;
        m_parameter = rhs.m_parameter;

    }
    return *this;
}

void GSlice::draw(QRootCanvas *pCanvas)
{
    Q_ASSERT(pCanvas != nullptr);

    m_pCanvas = pCanvas;

    frameChanged();

    m_pLow->Draw("same");
    m_pHigh->Draw("same");

}

double GSlice::getXLow() const
{
    return m_pLow->GetX1();
}

double GSlice::getXHigh() const
{
    return m_pHigh->GetX1();
}

void GSlice::draw()
{
    TFrame* pFrame = gPad->GetFrame();

    m_pLow->Draw("same");
    m_pHigh->Draw("same");

}
void GSlice::setXLow(double x)
{
    m_pLow->SetX1(x);
    m_pLow->SetX2(x);
}

void GSlice::setXHigh(double x)
{
    m_pHigh->SetX1(x);
    m_pHigh->SetX2(x);
}


void GSlice::frameChanged()
{
    Q_ASSERT(m_pCanvas != nullptr);

    auto pFrame = m_pCanvas->getCanvas()->GetFrame();

    frameChanged(pFrame);
}

void GSlice::frameChanged(TFrame* pFrame)
{
    Q_ASSERT(pFrame != nullptr);

    auto yLow  = pFrame->GetY1();
    auto yHigh = pFrame->GetY2();

    m_pLow->SetY1(yLow);
    m_pLow->SetY2(yHigh);

    m_pHigh->SetY1(yLow);
    m_pHigh->SetY2(yHigh);
}

void GSlice::nameChanged(const QString &name)
{
    m_name = name;
}
