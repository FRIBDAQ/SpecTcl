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
#include <string.h>

//////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////
// Implement the HistogramManager class.

/**
 * constructor:
 *   @param pMemory - pointer to mapped Xamine or mirrored memory.
 *   @param host    - Host in which the SpecTcl is running.
 *   @param restPort - Port on whichthe SpecTcl REST server is listening.
 */
HistogramManager::HistogramManager(void* pMemory, const char* host, int restPort) :
    m_pRefreshTimer(new THMTimer(1, this)),
    m_pXamineMemory(const_cast<volatile Xamine_shared*>(
        reinterpret_cast<Xamine_shared*>(pMemory))
    ),
    m_SpecTclHost(host),
    m_SpecTclRestPort(restPort)
{
    memset(m_pHistograms, 0, sizeof(m_pHistograms));
}
/**
 * destructor:
 *    - Kill the timer.
 *    - Kill all histograms that are currently defined.
 */
HistogramManager::~HistogramManager()
{
    m_pRefreshTimer->TurnOff();          // Pull it out of the event loop.
    delete m_pRefreshTimer;
    killAllHistograms();                 // Destroy the Root histos.
}

/**
 * start
 *    Start refreshing the histograms off the refresh timer
 */
void
HistogramManager::start()
{
    m_pRefreshTimer->TurnOn();              // Enable the timer.
}
/**
 * update
 *    For now this is a straightforward itration over m_pHistograms
 *    If the histogram changed, kill it and create it.
 *    @note killHistogram works properly on a null pointer.
 *    @note If spectrum i in spectrum memory does not have a spectrum,
 *           createHistgram does not create a new one.
 */
void
HistogramManager::update()
{
    for (int i = 0; i < XAMINE_MAXSPEC; i++) {
        if (histogramChanged(i)) {
            killHistogram(i);
            createHistogram(i);
        }
    }
}
/**
 * killAllHistograms
 *    Used in cleanup - kill all the Root histograms.
 */
void
HistogramManager::killAllHistograms()
{
    for (int i = 0; i < XAMINE_MAXSPEC; i++) {
        killHistogram(i);
    }
}