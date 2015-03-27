#ifndef HISTOGRAMLIST_H
#define HISTOGRAMLIST_H

#include "HistogramBundle.h"
#include <QFrame>
#include <QMap>
#include <QString>

class TH1;
class TCutG;

class HistogramList : public QObject
{
    Q_OBJECT
    
public:
    using iterator       = typename QMap<QString,HistogramBundle>::iterator;
    using const_iterator = typename QMap<QString,HistogramBundle>::const_iterator;

private:
    explicit HistogramList(QObject *parent = 0);
    HistogramList(const HistogramList&) = delete;
    ~HistogramList();

public:
    static HistogramList* getInstance()
    {
        if (m_instance == nullptr) {
            m_instance = new HistogramList;
        }
        return m_instance;
    }

    static QList<QString> histNames() { return m_hists.keys(); }
    static QMap<QString,HistogramBundle>::iterator begin() { return m_hists.begin();}
    static QMap<QString,HistogramBundle>::iterator end() { return m_hists.end();}

public slots:

    static bool histExists(const QString& name);
    static HistogramBundle* getHist(const QString& name);
    static void addHist(TH1& hist);


private:
    static HistogramList* m_instance;

    static QMap<QString,HistogramBundle> m_hists;
    static QMutex m_mutex;

};

#endif // HISTOGRAMLIST_H
