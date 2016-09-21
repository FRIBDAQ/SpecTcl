
#include <TAxis.h>
#include <TH2.h>

#include <iostream>

namespace Viewer
{

template<class H1Type>
int SubscribableH1<H1Type>::m_instanceCount = 0;


template<class H1Type>
SubscribableH1<H1Type>::SubscribableH1()
    : H1Type(),
      m_subscribers()
{
    m_instanceCount++;
//    std::cout << "SubscribableH1<H1Type>::instanceCount = " << m_instanceCount << std::endl;
}

//template<class H1Type>
//void SubscribableH1<H1Type>::Copy(H1Type& hist) const
//{
//    TH1::Copy(hist);
//}

template<class H1Type>
SubscribableH1<H1Type>::SubscribableH1(const char *name, const char *title,
                                       unsigned int nBins, double xMin, double xMax)
    : H1Type(name, title, nBins, xMin, xMax),
      m_subscribers()
{
    m_instanceCount++;
//    std::cout << "SubscribableH1<H1Type>::instanceCount = " << m_instanceCount << std::endl;
}

template<class H1Type>
SubscribableH1<H1Type>::SubscribableH1(const char *name, const char *title,
                                       unsigned int nBinsX, double xMin, double xMax,
                                       unsigned int nBinsY, double yMin, double yMax)
    : H1Type(name, title, nBinsX, xMin, xMax, nBinsY, yMin, yMax),
      m_subscribers()
{
    m_instanceCount++;
//    std::cout << "SubscribableH1<H1Type>::instanceCount = " << m_instanceCount << std::endl;
}

template<class H1Type>
SubscribableH1<H1Type>::~SubscribableH1()
{
    notifyAll();

    m_instanceCount--;
//    std::cout << "SubscribableH1<H1Type>::instanceCount = " << m_instanceCount << std::endl;
}

template<class H1Type>
void SubscribableH1<H1Type>::subscribe(H1Subscriber& subscriber)
{
    m_subscribers.insert(&subscriber);
}

template<class H1Type>
void SubscribableH1<H1Type>::unsubscribe(H1Subscriber& subscriber)
{
    auto subscribedElement = m_subscribers.find(&subscriber);
    if (subscribedElement != m_subscribers.end()) {
        m_subscribers.erase(subscribedElement);
    }
}

template<class H1Type>
bool SubscribableH1<H1Type>::isSubscribed(H1Subscriber &subscriber) {

    auto subscribedElement = m_subscribers.find(&subscriber);
    return (subscribedElement != m_subscribers.end());
}

template<class H1Type>
void SubscribableH1<H1Type>::notifyAll()
{
    for (auto& subscriber : m_subscribers) {
        subscriber->notify(*this);
    }
}

template<class H1Type>
void
SubscribableH1<H1Type>::setSubscribers(const std::set<H1Subscriber*>& subscribers)
{
    m_subscribers = subscribers;
}

template<class H1Type>
const std::set<H1Subscriber*>&
SubscribableH1<H1Type>::getSubscribers() const {
    return m_subscribers;
}

template<class H1Type>
SubscribableH1<H1Type>* CloneImpl(const TH2* pHist)
{
    const TAxis* pXaxis = pHist->GetXaxis();
    const TAxis* pYaxis = pHist->GetYaxis();
    return new SubscribableH1<H1Type>("pHist", pHist->GetTitle(),
                                        pHist->GetNbinsX(), pXaxis->GetXmin(), pXaxis->GetXmax(),
                                        pHist->GetNbinsY(), pYaxis->GetXmin(), pYaxis->GetXmax());
}

template<class H1Type>
SubscribableH1<H1Type>* CloneImpl(const TH1* pHist)
{
    const TAxis* pXaxis = pHist->GetXaxis();
    return new SubscribableH1<H1Type>("pHist", pHist->GetTitle(),
                                      pHist->GetNbinsX(), pXaxis->GetXmin(), pXaxis->GetXmax());
}

template<class H1Type>
SubscribableH1<H1Type>* SubscribableH1<H1Type>::Clone(const char* name) const
{
    SubscribableH1<H1Type>* pClone= CloneImpl<H1Type>(this);


    this->Copy(*pClone);

    if (strlen(name) > 0) {
        pClone->SetName(name);
    }

    pClone->setSubscribers(getSubscribers());

    return pClone;
}

} // end Viewer namespace
