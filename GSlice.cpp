#include "GSlice.h"
#include "QRootCanvas.h"
#include <TLine.h>
#include <TCanvas.h>
#include <TFrame.h>

GSlice::GSlice(QObject *parent, double xLow, double xHigh,
               QRootCanvas* pCanvas)
    :
    QObject(parent),
    m_name("__slice_in_progress__"),
    m_pLow(new TLine(xLow, 0, xLow, 1)),
    m_pHigh(new TLine(xHigh, 0, xHigh, 1)),
    m_pCanvas(pCanvas)
{
}

//GSlice::GSlice(const GSlice &rhs)
//    : QObject(rhs),
//      m_name(rhs.m_name),
//      m_pLow(new TLine(*rhs.m_pLow)),
//      m_pHigh(new TLine(*rhs.m_pHigh)),
//      m_pCanvas(rhs.m_pCanvas)
//{}

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
