#include "TestH1Subscriber.h"

namespace Viewer
{


TestH1Subscriber::TestH1Subscriber()
    : m_notified(false)
{
}

void TestH1Subscriber::notify()
{
    m_notified = true;
}

} // end Viewer namespace
