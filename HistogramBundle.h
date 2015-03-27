#ifndef HISTOGRAMBUNDLE_H
#define HISTOGRAMBUNDLE_H

#include <QMutex>
#include <QString>
#include <vector>

class TH1;
class TCutG;


class HistogramBundle {
private:
    QMutex* m_pMutex;
    TH1* m_pHist;
    std::vector<TCutG*> m_cuts2d;

public:
    HistogramBundle();
    HistogramBundle(QMutex& pMutex, TH1& pHist);

    void lock() const {
        m_pMutex->lock();
    }

    void unlock() const {
        m_pMutex->unlock();
    }

    TH1* hist() const {
        return m_pHist;
    }

    void addCut2D(TCutG* pCut);
    void draw(const QString& opt = QString());
};


#endif // HISTOGRAMBUNDLE_H
