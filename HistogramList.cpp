#include "HistogramList.h"
#include <memory>
#include <TH1.h>
#include <QString>
#include <QMap>

QMap<QString,TH1*> HistogramList::m_hists;
HistogramList* HistogramList::m_instance = nullptr;

HistogramList::HistogramList(QObject *parent) :
    QObject(parent)
{
}

HistogramList::~HistogramList()
{
    // free all of the hists
    for (auto pHist : m_hists) {
        delete pHist;
    }
}


TH1* HistogramList::getHist(const QString &name)
{
    auto iter = m_hists.find(name);
    if (iter!=m_hists.end()) {
        return (*iter);
    } else {
        return nullptr;
    }
}

void HistogramList::addHist(TH1& rHist)
{
    m_hists.insert(QString(rHist.GetName()),&rHist);
}

