#ifndef HISTOGRAMLIST_H
#define HISTOGRAMLIST_H

#include <QFrame>
#include <QMap>
#include <memory>
#include <QString>
#include <QList>
#include <QPair>
#include <QMutex>

class TH1;
typedef QPair<QMutex*,TH1*> GuardedTH1;


class GuardedHist {
private:
    QPair<QMutex*,TH1*> m_pair;

public:

    GuardedHist(QMutex& mutex, TH1& hist)
        : m_pair(qMakePair(&mutex,&hist))
    {}

    void lock() const {
        m_pair.first->lock();
    }

    void unlock() const {
        m_pair.first->unlock();
    };

    TH1* hist() const {
        return m_pair.second;
    }
};

template<class T> class LockGuard
{
private:
    T m_lock;
public:
    LockGuard(T obj) : m_lock(obj) {
        m_lock.lock();
    }

    ~LockGuard() {
        m_lock.unlock();
    }
};

template<class T> LockGuard<T> makeLockGuard(const T& obj) {
    return LockGuard<T>(obj);
}

class HistogramList : public QObject
{
    Q_OBJECT
    
public:
    using iterator       = typename QMap<QString,GuardedHist>::iterator;
    using const_iterator = typename QMap<QString,GuardedHist>::const_iterator;

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
    static QMap<QString,GuardedHist>::iterator begin() { return m_hists.begin();}
    static QMap<QString,GuardedHist>::iterator end() { return m_hists.end();}

public slots:

    static bool histExists(const QString& name);
    static GuardedHist getHist(const QString& name);
    static void addHist(TH1& hist);


private:
    static HistogramList* m_instance;

    static QMap<QString,GuardedHist> m_hists;
    static QMutex m_mutex;

};

#endif // HISTOGRAMLIST_H
