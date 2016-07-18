//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#ifndef SUBSCRIBABLEH1_H
#define SUBSCRIBABLEH1_H

#include <set>

namespace Viewer
{

/*!
 * \brief The H1Subscriber class
 *
 *  Minimum interface required by subscribers to the SubscribableH1 interface.
 */
class H1Subscriber {
public:

    /*!
     * \brief Method called when subscriber notified
     */
    virtual void notify() = 0;
};


/*! \brief Decorator class to TH1 that manages subscriptions
 *
 *  The need for this class arises from the fact that I need to keep track of
 *  the histogram clones that exist in HistogramBundle. Because the histogram
 *  clones are actually owned by the canvas, I need to know when to stop trying to
 *  update their contents during update operations. This class alerts subscribers
 *  when it destructs. It is intended to extend TH1 and TH2 derived objects. We use
 *  SFINAE to avoid writing duplicate code to handle all of these types. Typical
 *  use cases are:
 *
 *  \code
 *  SubscribabeH1<TH1D> hist1d("myHist", "Histogram Title; x label; y label; 10, 0, 10);
 *  SubscribabeH1<TH2D> hist2d("myHist", "Histogram Title; x label; y label; 10, 0, 10, 10, 0, 10);
 *  \endcode
 */
template<class H1Type>
class SubscribableH1 : public H1Type
{
private:
    std::set<H1Subscriber*> m_subscribers;

public:
    /*!
     * \brief Constructor for TH1 objects
     *
     * \param name  - name of hist
     * \param title - title of hist
     * \param nBins - numbers of bins on the x-axis
     * \param xMin  - lower limit of range
     * \param xMax  - upper limit of range
     */
    SubscribableH1(const char* name, const char* title,
                   unsigned int nBins, double xMin, double xMax);

    /*!
     * \brief Constructor for TH2 objects
     *
     * \param name      - name of hist
     * \param title     - title of hist
     * \param nBinsX    - number of bins on x axis
     * \param xMin      - lower limit of x axis
     * \param xMax      - upper limit of x axis
     * \param nBinsY    - number of bins on y axis
     * \param yMin      - lower limit of y axis
     * \param yMax      - upper limit of y axis
     */
    SubscribableH1(const char* name, const char* title,
                   unsigned int nBinsX, double xMin, double xMax,
                   unsigned int nBinsY, double yMin, double yMax);

    /*!
     * \brief Notify and destruct
     *
     * All subscribers are notified.
     */
    virtual ~SubscribableH1();

    /*!
     * \brief Subscribe to receive notifications of destruction
     *
     * \param subscriber    -   reference to the subscriber
     */
    void subscribe(H1Subscriber& subscriber);

    /*!
     * \brief Unsubscribe from notifications
     *
     * It is not an error to unsubscribe a subscriber that is not already
     * subscribed.
     *
     * \param subscriber - subscriber to remove
     */
    void unsubscribe(H1Subscriber& subscriber);

    /*!
     * \brief Check whether a specific subscriber is already subscribed
     *
     * \param subscriber    - the subscriber in question
     *
     * \return boolean
     * \retval true - subscribed, false - not subscribed
     */
    bool isSubscribed(H1Subscriber& subscriber);

    void setSubscribers(const std::set<H1Subscriber*>& subscribers);
    const std::set<H1Subscriber*>& getSubscribers() const;


    /*!
     * \brief Notify all subscribers
     *
     * This is called by the destructor but it can be called freely.
     */
    void notifyAll();

    virtual SubscribableH1<H1Type>* Clone(const char* name);
};

} // end Viewer namespace

#include "SubscribableH1.hpp"

#endif // SUBSCRIBABLETH1_H
