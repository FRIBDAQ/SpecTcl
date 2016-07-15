

namespace Viewer
{

template<class H1Type>
SubscribableH1<H1Type>::SubscribableH1(const char *name, const char *title,
                                       unsigned int nBins, double xMin, double xMax)
    : H1Type(name, title, nBins, xMin, xMax),
      m_subscribers()
{
}

template<class H1Type>
SubscribableH1<H1Type>::SubscribableH1(const char *name, const char *title,
                                       unsigned int nBinsX, double xMin, double xMax,
                                       unsigned int nBinsY, double yMin, double yMax)
    : H1Type(name, title, nBinsX, xMin, xMax, nBinsY, yMin, yMax),
      m_subscribers()
{
}

template<class H1Type>
SubscribableH1<H1Type>::~SubscribableH1()
{
    notifyAll();
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
        subscriber->notify();
    }
}



} // end Viewer namespace
