#ifndef SUBSCRIBABLEH1_H
#define SUBSCRIBABLEH1_H

#include <set>

namespace Viewer
{

class H1Subscriber {
public:
    virtual void notify() = 0;
};


template<class H1Type>
class SubscribableH1 : public H1Type
{
private:
    std::set<H1Subscriber*> m_subscribers;

public:
    SubscribableH1(const char* name, const char* title, unsigned int nBins, double xMin, double xMax);
    SubscribableH1(const char* name, const char* title, unsigned int nBinsX, double xMin, double xMax,
                   unsigned int nBinsY, double yMin, double yMax);

    virtual ~SubscribableH1();

    void subscribe(H1Subscriber& subscriber);
    void unsubscribe(H1Subscriber& subscriber);

    bool isSubscribed(H1Subscriber& subscriber);

    void notifyAll();
};

} // end Viewer namespace

#include "SubscribableH1.hpp"

#endif // SUBSCRIBABLETH1_H
