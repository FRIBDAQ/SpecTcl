#ifndef HISTOGRAMLIST_H
#define HISTOGRAMLIST_H

#include <QFrame>
#include <QMap>
#include <memory>
#include <QString>
#include <QList>

class TH1;

// A Singleton

class HistogramList : public QObject
{
    Q_OBJECT
    
public:
    using iterator       = typename QMap<QString,TH1*>::iterator;
    using const_iterator = typename QMap<QString,TH1*>::const_iterator;

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
    static QMap<QString,TH1*>::iterator begin() { return m_hists.begin();}
    static QMap<QString,TH1*>::iterator end() { return m_hists.end();}

public slots:

    static TH1* getHist(const QString& name);
    static void addHist(TH1& hist);


private:
    static HistogramList* m_instance;

    static QMap<QString,TH1*> m_hists;


};

#endif // HISTOGRAMLIST_H
