#ifndef TESTH1SUBSCRIBER_H
#define TESTH1SUBSCRIBER_H

#include <SubscribableH1.h>

namespace Viewer
{

class TestH1Subscriber : public H1Subscriber
{

private:
    bool m_notified;

public:
    TestH1Subscriber();
    bool isNotified() const { return m_notified; }
    void notify();
};

} // end Viewer namespace

#endif // TESTH1SUBSCRIBER_H
