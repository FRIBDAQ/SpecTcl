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

/** @file:  RootTreeSink.cpp
 *  @brief: Implement a data sink that knows how to write root trees to TFile
 *
 */

#include "RootTreeSink.h"
#include <TFile.h>
#include "TreeBuilder.h"
#include "GateContainer.h"
#include <SpecTcl.h>
#include <TFile.h>
#include <TDirectory.h>
#include <Event.h>
#include <EventList.h>
#include <stdio.h>


#include <sstream>
#include <fnmatch.h>                 // For parameter pattern matching.


/**
 * constructor
 *   @param name     - name of the tree to create.
 *   @param patterns - Vector of parameter name patterns.
 *   @param gate     - Gate container of the condition under which items are
 *                      added to the tree.
 *   @note - We just initialize the data members as the tree gets built
 *           by OnBegin.
 */
RootTreeSink::RootTreeSink(
    std::string name, const std::vector<std::string>& patterns,
    CGateContainer* m_pGate
) :
    m_pFile(0),
    m_pTree(0),
    m_Gate(*m_pGate),
    m_parameterPatterns(patterns),
    m_treeName(name)
{
    
}
/**
 * destructor.
 *    Write if the file is open and destroy all the dynamic bits and pieces.
 */
RootTreeSink::~RootTreeSink()
{
    tearDown();
}

/**
 * OnOpen
 *    A new file has been opened for us.
 *    - Complain if we know the file is already open.
 *    - Create a new tree.
 *  @param pNewFile - the file that's been opened for our trees.
 */
void
RootTreeSink::OnOpen(TFile* pNewFile)
{
    // Not sure if I can sanely tear down the tree when the file has been
    // yanked out from underneath it, so:
    
    if (m_pFile) {
        std::cerr << "** Warning -- RootTreeSink::OnOpen - Root file has not been closed \n";
        std::cerr << "**            This will result in a small memory leak\n";
    }
    m_pFile = pNewFile;
    createTree();
}
/**
 * OnAboutToClose
 *     The guy managing the TFile* is about to close it.
 *     teardown our tree and null out the tree/file pointers.
 */
void
RootTreeSink::OnAboutToClose()
{
    
    tearDown();            // Nulls the pointers for us too.
}

/**
 * operator()
 *    Called when a set of events must be processed:
 *    - If no file/tree has beens set up, set it up.
 *    - Process each event in the list.
 *
 *  @param rEvents - events to process (basically a vector of event references).
 */
void
RootTreeSink::operator()(CEventList& rEvents)
{
    // Just silently ignore the data if we've not got a file.
    
    if (m_pFile) {
    
        // Process the events one at a time.
        
        for (int i =0; i < rEvents.size(); i++) {
            if(rEvents[i]) {
                    (*this)(*(rEvents[i]));
            } else {
                break;               // Three aren't any more events in the list.
            }
        }
    }
}
/**
 *  OnBegin
 *    A begin run has happened.. if the file is open, we teardown the tree, 
 * close the file then create a  new file and invoke OnOpen to do the rest of the
 * work.
 *   @param runNumber - number of the new run.
 *   @param title     - Title of the run Title s the root file.
 *
 * @note using UPDATE supports the case where we have multiple begin runs in a source
 * as we do for event built data.
 */
void
RootTreeSink::OnBegin(unsigned runNumber, const char* title) {
    if (m_pFile) {
        auto pFile = m_pFile;
        OnAboutToClose();
        delete pFile;
    }
    // A new filename of the form run-nnnn.root:


    char filename[1000];       // More than enough I think:
    snprintf(filename, sizeof(filename), "%s-run-%04u.root", m_treeName.c_str(), runNumber);
    TFile* pFile = new TFile(filename, "UPDATE", title);
    OnOpen(pFile);
}
/**
 * OnEnd
 *   An end run was detected - the file, if opened is closed.
 * 
 * @param runNumber - number of the run being closed.
 * @param title  - Title of the run.
 * 
 * @note - In this implementation if there multiple END run items, the event data
 * that might be intermixed between them (if barrier synch is off) will be lost from
 * the output file.
 */
void
RootTreeSink::OnEnd(unsigned runNumbver, const char* title) {
    if (m_pFile) {
        auto pFile = m_pFile;
        OnAboutToClose();
        pFile->Close();
        delete pFile;
    }
}

/*----------------------------------------------------------------------------
 *  Private methods (utilities).
 */

/**
 * operator()
 *    Process a single event.  The infrastructure to create a tree is already
 *    assumed to be in place.
 *    - Ensure the gate is made.
 *    - If so, fill the tree:
 *
 *  @param event  - the event to process.
 */
void
RootTreeSink::operator()(CEvent& event)
{
    (m_Gate)->RecursiveReset();            // Clear gate and what it might depend on.
    if ((*m_Gate)(event)) {
        m_pTree->Fill(event);
    }
}
void
RootTreeSink::createTree()
{
    // Figure out which parameters match the patterns:
    
    SpecTcl* pApi = SpecTcl::getInstance();
    std::vector<ParameterTree::ParameterDef> params;
    for (auto p = pApi->BeginParameters(); p != pApi->EndParameters(); p++) {
        std::string name = p->first;
        unsigned    id   = p->second.getNumber();
        for (int i = 0; i < m_parameterPatterns.size(); i++) {
            if (! fnmatch(m_parameterPatterns[i].c_str(), name.c_str(), 0)) {
                // Match:
                
                params.push_back(ParameterTree::_ParameterDef(name.c_str(), id));
                break;                      // No more patterns to check.
            }
        }
    }
    // Construct the tree:
    
    m_pTree = new SpecTclRootTree(m_treeName, params);
}
/**
 * tearDown
 *    If the file is open and the tree exists, flush everything out to file.
 *    destroy the tree and destroy the file object.
 *
 *    Note m_pFile is anaged externally so we don't do anything to it.
 */
void
RootTreeSink::tearDown()
{
    if (m_pFile) {
        m_pFile->Flush();
        delete m_pTree; 
        
        m_pTree = nullptr;
        m_pFile = nullptr;
    }
}
