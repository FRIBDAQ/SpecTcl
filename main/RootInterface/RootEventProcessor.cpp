/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  RootEventProcessor.h
 *  @brief: RootEventProcessor.cpp
 */

#include "RootEventProcessor.h"
#include "RootTreeSink.h"
#include <BufferDecoder.h>
#include <Exception.h>
#include <stdexcept>

#include <iostream>
#include <sstream>
#include <algorithm>

#include <TFile.h>
#include <TDirectory.h>

/**
 *  constructor
 *     Just initialize stuff so we know there's no file open.
 */
RootEventProcessor::RootEventProcessor() :
    m_pFile(0), m_nNumBeginSequence()
{}

/**
 *  The event processor lives for the life of the program once
 *  created. Sure we should do something at destruction time about hanging
 *  sinks but there's not really much point to it as destruction coincides
 *  with the death of the program.
 */

/**
 * OnBegin
 *    If there's an open file (no end because e.g. we were reattached),
 *    -  Invoke all the OnAboutToClose methods.
 *    -  Close the file.
 *    Then:
 *    -  Open a new file using the 'standard' filename.
 *    -  Invoke the OnOpen method for each sink.
 *    
 * @param rA  - references the analyzer object.  We don't need this.
 * @param rB  - references the buffer decoder object. We'll get run number
 *              and title from this.
 * @return kfTRUE - unless an exception is thrown in which case kfFALSE is
 *              returned along with a message to stderr.
 * @note in event built data there could be more than one begin per run.
 *       in that case we'll open/close for all but the last begin and all will
 *       still be (inefficiently) good.
 */
Bool_t
RootEventProcessor::OnBegin(CAnalyzer& rA, CBufferDecoder& rB) 
{
    Bool_t result = kfTRUE;
    std::string message;
    
    UInt_t run        = rB.getRun();
    std::string title = rB.getTitle();
    
    try {
        // If the file is open clean up:
        
        if (m_pFile) {
            invokeOnAboutToClose();
            m_pFile->Write();
            delete m_pFile;
            m_pFile = nullptr;             // in case something bad happens.
        }
        // Construct a new filename, open the file and invoke the OnOpen's.
        std::string olddir = gDirectory->GetPath();
        gDirectory->Cd("/");
        
        std::stringstream sFilename;
        sFilename << "run-" << run << ".root";
        m_pFile = new TFile(sFilename.str().c_str(), "UPDATE");
        
        invokeOnOpen();
        gDirectory->Cd(olddir.c_str());
        
    }
    catch (CException& e) {
        message = "CException caught RootEventProcessor::OnBegin ";
        message += e.ReasonText();
        result  = kfFALSE;
    }
    catch (std::exception& e) {
        message = "std::exception caught RootEventProcessor::OnBegin ";
        message += e.what();
        result  = kfFALSE;
    }
    catch (const char* msg) {
        message = "const char* exception caught RootEventProcessor::OnBegin ";
        message += msg;
        result  = kfFALSE;
    }
    catch (std::string msg) {
        message = "std::string exception caught RootEventProcessor::OnBegin ";
        message += msg;
        result  = kfFALSE;
    }
    catch (...) {
        message = "Unanticipated exception caught by RootEventProcessor::OnBegin";
        result  = kfFALSE;
    }
    if (!result) {
        std::cerr << message << std::endl;
    }
    return result;
}
/**
 * OnEnd
 *    Called at the end of the run.
 *    If a file is open;
 *    - Invoke about to close on all sinks.
 *    - Close the file.
 *
 * @param rA  - Reference to the analyzer.
 * @param rB  - Reference to the buffer decoder.
 * @note - both parameters are unused.
 * @return kfTRUE unless an exception is thrown and then kfFALSE.
 * 
 */
Bool_t
RootEventProcessor::OnEnd(CAnalyzer& rA, CBufferDecoder& rB)
{
    Bool_t result = kfTRUE;
    std::string message;
    
    try {
        if (m_pFile) {              /// there could be more than one end/run
            m_pFile->Write();
            invokeOnAboutToClose();
            delete m_pFile;
            m_pFile = nullptr;
        }
    }
    catch (CException& e) {
        message = "CException in RootEventProcessor::OnEnd - ";
        message += e.ReasonText();
        result  = kfFALSE;
    }
    catch (std::exception & e) {
        message = "std::exception caught in RootEventProcessor::OnEnd - ";
        message += e.what();
        result  = kfFALSE;
    }
    catch (const char* msg) {
        message = "const char* caught in RootEventProcessor::OnEnd - ";
        message += msg;
        result = kfFALSE;
    }
    catch (std::string msg) {
        message = "std::string caught in RootEventProcessor::OnEnd - ";
        message += msg;
        result += kfFALSE;
    }
    catch (...) {
        message += "Unanticipated exception type caught in RootEventProcessor::OnEnd";
        result = kfFALSE;
    }
    
    if (!result) {
        std::cerr << message << std::endl;
    }
    return result;
}
/**
 * operator()
 *   This is only used because we may get created or attached in mid
 *   run.   If there's no open file, we  need to open a new file
 *   and let the event processors open their new file.
 */
Bool_t
RootEventProcessor::operator()(
    const Address_t pEvent, CEvent& rEvent, CAnalyzer& rAnalyzer,
	CBufferDecoder& rDecoder
)
{
    if (!m_pFile) {
        std::string olddir = gDirectory->GetPath();
        gDirectory->Cd("/");
        m_pFile = new TFile(defaultFilename().c_str(), "UPDATE");
        invokeOnOpen();
        gDirectory->Cd(olddir.c_str());
        
    }
    return kfTRUE;
}
/**
 * OnEventSourceOpen
 *    Invoked when a new event source is being opened.  If the file is
 *    open we close it as this may happen in the middle of analyzing a run.
 */
Bool_t
RootEventProcessor::OnEventSourceOpen(std::string name)
{
    if (m_pFile) {
        invokeOnAboutToClose();
        delete m_pFile;
        m_pFile = nullptr;
    }
}


/**
 * addTreeSink
 *    Add a new tree sink to the managed sinks.
 *
 *   @param name - name of the sink.
 *   @param sink - pointer to the sink to and.
 *   @throw std::invalid_argument - if the name already exists.
 */
void
RootEventProcessor::addTreeSink(const char* name, RootTreeSink* sink)
{
    // If exists that's a problem:
    
    if (m_sinks.find(name) != m_sinks.end()) {
        std::string msg = name;
        msg += " already exists.";
        throw std::invalid_argument(msg);
    }
    m_sinks[name] = sink;
    
    // If there's an open file, add this to it:
    
    if (m_pFile) {
        sink->OnOpen(m_pFile);
    }
}
/**
 * removeSink
 *    Removes a sink by name from the collection of sinks.
 *
 * @param name - name of the sink to remove.
 * @return RootTreeSink* - pointer to the removed sink.  Up to the caller to do
 *                         any storage management (e.g. delete).
 * @throw std::invalid_argument if there's no such item.
 */
RootTreeSink*
RootEventProcessor::removeTreeSink(const char* name)
{
    auto p = m_sinks.find(name);
    if (p == m_sinks.end()) {
        // not found.
        
        std::string msg = name;
        msg            += " not found";
        throw std::invalid_argument(msg);
    }
    RootTreeSink* result = p->second;
    m_sinks.erase(p);
    
    // If there's an open file, flush  the file and close out the tree:
    
    if (m_pFile) {
        m_pFile->Write();
        result->OnAboutToClose();       // Not actually but for this guy yes.
    }
    
    return result;
    
}
/**
 * begin
 *    Return a const iterator to the beginning of the collection.
 *  @return std::map<std::string, RootTreeSink*>::const_iterator
 */
std::map<std::string, RootTreeSink*>::const_iterator
RootEventProcessor::begin() const
{
    return m_sinks.begin();
}
/**
 * end
 *  @return std::map<std::string, RootTreeSink*>::const_iterator -
 *          const iterator just off the end of the collection of sinks.
 *
 */
std::map<std::string, RootTreeSink*>::const_iterator
RootEventProcessor::end() const
{
    return m_sinks.end();
}

/**
 * defaultFilename
 *    @return std::string - A default filename used if no begin run item
 *                          defines the TFile name.
 */
std::string
RootEventProcessor::defaultFilename()
{
    std::stringstream sName;
    sName << "SpecTcl-" << m_nNumBeginSequence++ << ".root";
    return sName.str();
}
/**
 * invokeOnOpen
 *    For each event sink we manage, invoke its OnOpen method passing in
 *    out m_pFile pointer.
 */
void
RootEventProcessor::invokeOnOpen()
{
    // this is a job for for_each along with a lambda:
    
    TFile* pFile = m_pFile;
    std::for_each (m_sinks.begin(), m_sinks.end(),
        [pFile](std::pair<std::string, RootTreeSink*> p) {
            p.second->OnOpen(pFile);
        }
    );
}
/**
 * invokOnAboutToClose
 *    for each event sink, invoke its OnAboutToClose method.
 */
void
RootEventProcessor::invokeOnAboutToClose()
{
    // another job for for_each and a lambda:
    
    std::for_each (m_sinks.begin(), m_sinks.end(),
        [](std::pair<std::string, RootTreeSink*> p) {
            p.second->OnAboutToClose();
        }
    );
}