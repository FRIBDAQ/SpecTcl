#include "HistogramBundle.h"
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <TH1.h>
#include <TCutG.h>

HistogramBundle::HistogramBundle()
    : m_pMutex(nullptr),
      m_pHist(nullptr),
      m_cuts2d()
{}
HistogramBundle::HistogramBundle(QMutex& pMutex, TH1& pHist)
    : m_pMutex(&pMutex),
      m_pHist(&pHist),
      m_cuts2d()
{}

void HistogramBundle::addCut2D(TCutG* pCut) {
    QMutexLocker lock(m_pMutex);
    m_cuts2d.push_back(pCut);
}

void HistogramBundle::draw(const QString& opt) {
    const char* cOpts = opt.toAscii().constData();
    m_pMutex->lock();
    m_pHist->Draw(cOpts);
    m_pMutex->unlock();

    for (auto cut : m_cuts2d) {
        cut->Draw("same");
    }
}
