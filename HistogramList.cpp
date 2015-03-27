#include "HistogramList.h"
#include <memory>
#include <TH1.h>
#include <QString>
#include <QMap>
#include <QMutexLocker>
#include <stdexcept>

HistogramList* HistogramList::m_instance = nullptr;

QMap<QString,HistogramBundle> HistogramList::m_hists;
QMutex HistogramList::m_mutex;

HistogramList::HistogramList(QObject *parent) :
    QObject(parent)
{}

HistogramList::~HistogramList()
{
    // free all of the hists
    for (auto pair : m_hists) {

        // do not delete a hist if it is being used.
        pair.lock();
        delete pair.hist();
        pair.unlock();

        // delete the hist's mutex
        delete pair.hist();
    }
}


bool HistogramList::histExists(const QString &name)
{
    QMutexLocker lock(&m_mutex);
    auto iter = m_hists.find(name);
    return (iter!=m_hists.end());
}


HistogramBundle* HistogramList::getHist(const QString &name)
{
    QMutexLocker lock(&m_mutex);
    auto iter = m_hists.find(name);
    if (iter!=m_hists.end()) {
        return &(*iter);
    } else {
        throw std::runtime_error("Requested histogram not found");
    }
}

void HistogramList::addHist(TH1& rHist)
{
    QString name(rHist.GetName());

    if (histExists(name)) {
        return;
    } else {
        QMutexLocker lock(&m_mutex);

        m_hists.insert(name, HistogramBundle(*(new QMutex),rHist));
    }
}

