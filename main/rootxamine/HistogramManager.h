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

/** @file:  HistogramManager.h
 *  @brief: Define class to maintain Root histograms from SpecTcl memory.
 */
#ifndef HISTOGRAMMANAGER_H
#define HISTOGRAMMANAGER_H
/**
 * @class HistogramManager
 *    This class maintains a set of root histograms that are congruent
 *    with the histograms that are stored in an Xamine Shared memory.
 *    As simple approach is taken where the class holds an array of
 *    TH1* pointers that is as large as the number of histograms
 *    Xamine shared memory can  hold.
 *    Every second (a root TTimer object is encapsulated),
 *    We pass through the set of Xamine spectra and corresopnding
 *    Histogram objects and:
 *    -   If Xamine undefined an existing spectrumwe delete the histogram.
 *    -   If Xamine created a new histogram where none was we create it.
 *    -   If Xamine modified an existing histogram we delete and then
 *        create.
 *    In this initial implementation, we don't bother with the SpecTcl
 *    REST server.  We are, howver given the port in case in the future
 *    we want to optimize the update by establishing a histogram trace
 *    and using that to home in on what actually changed since the last
 *    timer fired.
 *    
 * @note - constructing the histogram manager simply gets the
 *         ball rolling.  That is on return from the constructor,
 *         all the internal data structures have been created and initialized
 *         the timer created
 *         The start method must then be called to actually enable the timer.
 */
#include <TTimer.h>
#include <TH1.h>
#include <xamineDataTypes.h>

#include <string>

class HistogramManager
{
private:
    TTimer*                   m_pRefreshTimer;
    volatile Xamine_shared*   m_pXamineMemory;
    std::string               m_SpecTclHost;
    int                       m_SpecTclRestPort;
    TH1*                      m_pHistograms[XAMINE_MAXSPEC];
    
public:
    
    HistogramManager(void* pMemory, const char* host, int restPort);
    ~HistogramManager();
    
    void start();
    void update();
private:
    void killAllHistograms();
    void killHistogram(int index);
    void createHistogram(int index);
    bool histogramChanged(int index);
};

#endif                               // include guard.