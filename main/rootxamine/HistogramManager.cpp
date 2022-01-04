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
#include <stdexcept>

#include <TH2I.h>
#include <TH2S.h>
#include <TH2C.h>

#include <TH1I.h>
#include <TH1S.h>

#include <TDirectory.h>

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
    for(int i =0; i < XAMINE_MAXSPEC; i++) {
        m_CurrentTypes[i] = undefined;
    }
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
/**
 * killHistogram
 *    Destroy a histogram given its slot number.
 * @param index - the Xamine memory slot that the histogram
 *                corresponds to
 */
void
HistogramManager::killHistogram(int index)
{
    if (m_pHistograms[index]) {
        clearStorage(index);
        delete m_pHistograms[index];
        m_pHistograms[index] = nullptr;   // No histogram present.
        m_CurrentTypes[index] = undefined;
    }
}
/**
 * histogramChanged
 *    @param index - index of a histogram in shared memory.
 *    @return bool - true if the histogram has changed.
 *    @note now that we have the current histogram type that's a
 *          matter of asking if:
 *          *   The histogram type has changed and if so true.
 *          *   The histogram dimensions have changed (if so true).
 *          *   The histogram axis definition has changed (if so true).
 *          *   Otherwise false.
 */
bool
HistogramManager::histogramChanged(int index)
{
    if (m_CurrentTypes[index] != m_pXamineMemory->dsp_types[index]) {
        return true;
    }
    // This protects against checking any further against unused spectra
    // If this test matches the spectrum was and is still undefined
    // and this is the only case where m_pHistograms[index] is a nullptr.
    
    if (m_CurrentTypes[index] == undefined) {
        return false;
    }
    
    // All spectra have x dimension and x axis specs:
    
    TH1* pHist = m_pHistograms[index];
    if (pHist->GetNbinsX() != m_pXamineMemory->dsp_xy[index].xchans) {
        return true;
    }
    TAxis* pX = pHist->GetXaxis();
    if (
        (pX->GetXmin() != m_pXamineMemory->dsp_map[index].xmin) ||
        (pX->GetXmax() != m_pXamineMemory->dsp_map[index].xmax)
    ) {
        return true;           // Y axis spec changed.
    }
    // Only 2d spectra have a y axis.  I don't trust Root
    // to give me a null ptr if 1d so:
    
    
    spec_type t = m_CurrentTypes[index];
    if ((t == twodlong) || (t == twodword) || (t == twodbyte)) {
        
        if (pHist->GetNbinsY() != m_pXamineMemory->dsp_xy[index].ychans) {
            return false;     // # Y bins changed.
        }
        TAxis* pY = pHist->GetYaxis();
        if (
            (pY->GetXmin() != m_pXamineMemory->dsp_map[index].ymin) ||
            (pY->GetXmax() != m_pXamineMemory->dsp_map[index].ymax)
        ) {
            return true;     // Has y axis and it changed.
        }
    }
    
    return false;                 // unchanged.
    
}
/**
 * createHistogram
 *    Based on the appropriate histogram type in the Xamine shared
 *    memory, create a Root histogram that's got its storage
 *    bound to it.
 * @param index -index of the histogram slot in Xamine.
 * @note this code assumes that killHistogram on this index has already
 *       been called.  This is correct if this is called from
 *       update().
 */
void
HistogramManager::createHistogram(int index)
{
    
    // Do nothing if the spectrum is undefined:
    
    spec_type stype = m_pXamineMemory->dsp_types[index];
    if (stype == undefined) return;
    std::string olddir = gDirectory->GetPath();
    gDirectory->Cd("/");
    
    // Extract the axis definition and binning of the
    // new histogram... what we do after that depends on
    // the histogram type sadly.
    // Note that in the 1d case the y channels an axis specs will be
    // ignored.
    // 
    
    unsigned xbins  = m_pXamineMemory->dsp_xy[index].xchans;
    unsigned ybins  = m_pXamineMemory->dsp_xy[index].ychans;
    
    float xmin      = m_pXamineMemory->dsp_map[index].xmin;
    float xmax      = m_pXamineMemory->dsp_map[index].xmax;
    float ymin      = m_pXamineMemory->dsp_map[index].ymin;
    float ymax      = m_pXamineMemory->dsp_map[index].ymax;
    
    const char* pName = const_cast<const char*>(m_pXamineMemory->dsp_titles[index]);
    const char* title = const_cast<const char*>(m_pXamineMemory->dsp_info[index]);
    size_t offset     = m_pXamineMemory->dsp_offsets[index];
    
    switch (stype) {
        case twodlong:             // TH2I
            {
                auto pSpec = new TH2I(
                    pName, title,
                    xbins, xmin, xmax,
                    ybins, ymin, ymax
                );
                pSpec->Adopt(0,nullptr);  // Free Root's storage...
                pSpec->fN = xbins*ybins;
                pSpec->fArray = reinterpret_cast<Int_t*>(const_cast<uint32_t*>(
                    &(m_pXamineMemory->dsp_spectra.XAMINE_l[offset])
                ));
                m_pHistograms[index] = pSpec;
            }
            break;
        case onedlong:                    // TH1I
            {
                auto pSpec = new TH1I(
                    pName, title, xbins, xmin, xmax
                );
                pSpec->Adopt(0, nullptr);
                pSpec->fN = xbins;
                pSpec->fArray =reinterpret_cast<Int_t*>(const_cast<uint32_t*>(
                    &(m_pXamineMemory->dsp_spectra.XAMINE_l[offset])
                ));
                m_pHistograms[index] = pSpec;
            }
            break;
        case onedword:                    // TH1S
            {
                auto pSpec = new TH1S(
                    pName, title, xbins, xmin, xmax
                );
                pSpec->Adopt(0, nullptr);
                pSpec->fN = xbins;
                pSpec->fArray = reinterpret_cast<Short_t*>(const_cast<uint16_t*>(
                    &(m_pXamineMemory->dsp_spectra.XAMINE_w[offset])
                ));                    
                m_pHistograms[index] = pSpec;
            }
            break;
        case twodword:                // TH2S;
            {
                auto pSpec = new TH2S(
                    pName, title,
                    xbins, xmin, xmax,
                    ybins, ymin, ymax
                );
                pSpec->Adopt(0,nullptr);  // Free Root's storage...
                pSpec->fN = xbins*ybins;
                pSpec->fArray = reinterpret_cast<Short_t*>(const_cast<uint16_t*>(
                    &(m_pXamineMemory->dsp_spectra.XAMINE_w[offset])
                ));                
                m_pHistograms[index] = pSpec;
            }
            break;
        case twodbyte:             // TH2C
            {
                auto pSpec = new TH2C(
                    pName, title,
                    xbins, xmin, xmax,
                    ybins, ymin, ymax
                );
                pSpec->Adopt(0,nullptr);  // Free Root's storage...
                pSpec->fN = xbins*ybins;
                pSpec->fArray = reinterpret_cast<Char_t*>(const_cast<uint8_t*>(
                    &(m_pXamineMemory->dsp_spectra.XAMINE_b[offset])
                ));
                m_pHistograms[index] = pSpec;
            }
            break;
        default:
            gDirectory->Cd(olddir.c_str());
            throw std::domain_error(
                "HistogramManager::createHistogram - unrecognized spectrum type in Xamine memory"
            );
                       
    }
    gDirectory->Cd(olddir.c_str());
    
}