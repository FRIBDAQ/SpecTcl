#ifndef TESTH1SUBSCRIBER_H
#define TESTH1SUBSCRIBER_H

#include <SubscribableH1.h>

namespace Viewer
{

/*!
 * \brief A testing implement of H1Subscriber
 *
 */
class TestH1Subscriber : public H1Subscriber
{

private:
    bool m_notified;    ///< flag for whether it has been notified

public:
    /*!
     * \brief Constructor
     *
     * By default, the construction leaves this in a state that isNotified()
     * returns false.
     */
    TestH1Subscriber();

    /*! \return whether it has been notified
     */
    bool isNotified() const { return m_notified; }

    /*!
     * \brief sets m_notified to true
     */
    void notify();
};

} // end Viewer namespace

#endif // TESTH1SUBSCRIBER_H
