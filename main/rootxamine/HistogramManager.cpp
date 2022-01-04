/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  HistogramManager.cpp
 *  @brief: Implementation of the HistogramManager class.
 */
#include "HistogramManager.h"
/**
 * THMTimer - histogram manager time class definition and implementation.
 *            The purpose of instances of this class is to drive the
 *            updates of the TH1 derived objects that represent
 *            Xamine shared memory histograms.
 */
class THMTimer : public TTimer {
private:
    HistogramManager* m_pManager;
public:
    THMTimer(int seconds, HistogramManager* pManager);
    virtual ~THMTimer();
    
    virtual Bool_t Notify();     // Called when the timer fires.
};
/**
 * Construct:
 *   @param seconds -number of seconds between pings.
 *   @param pManager - Pointer to the manager object.
 */
THMTimer::THMTimer(int seconds, HistogramManager* pManager) :
    TTimer(seconds * 1000), 
    m_pManager(pManager)
{}
/**
 * destructor - ensure the timer is turned off in case root is silly enough
 *              not to do that for me.
 */
THMTimer::~THMTimer()
{
    TurnOff();    
}
/**
 * Notify
 *    Called when the timer is triggered.  We're called from the
 *    ROOT event loop  so that's ensured to be the same thread
 *    as the main thread.
 *    As the Root reference says we need to invoke Reset to reschedule.
 */
Bool_t THMTimer::Notify()
{
    m_pManager->update();        // Purpose of the timer....
    Reset();                     // Get ourself rescheduled.
    return kTRUE;               // A real guess here.
}