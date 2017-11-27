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
#include <TDirectory.h>
#include <Event.h>
#include <EventList.h>


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
    m_nNumBeginSequence(0),
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
 * OnBegin
 *    Destroy any existing stuff.
 *    Open a Root file based on the run number.
 *    Construct the Tree based on the parameter filters.
 *    Set the appropriate member variables to be ready for events.
 */
void
RootTreeSink::OnBegin(unsigned run, std::string title)
{
    tearDown();                    // in case there is no end run.
    
    // construct the name of the new root file and open it.
    
    std::stringstream sName;
    sName << "run-" << run << ".root";
    
    std::string olddir = gDirectory->GetPath();
    gDirectory->Cd("/");
    
    openFile(sName.str());
    
    // Build the tree:
    
    createTree();
    gDirectory->Cd(olddir.c_str());
    
}
/**
 * OnEnd
 *    Tear down the infrastructure for making/writing the tree and
 *    so on.
 */
void
RootTreeSink::OnEnd()
{
    tearDown();
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
    
    // IF needed open a file and construct the tree:
    
    if (! m_pFile) {

        std::string olddir = gDirectory->GetPath();
        gDirectory->Cd("/");
        openFile(defaultFilename().c_str());
        createTree();
        gDirectory->Cd(olddir.c_str());
        
    }
    // Process the events one at a time.
    
    for (int i =0; i < rEvents.size(); i++) {
        (*this)(*(rEvents[i]));
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
/**
 * openFile
 *    Create a new open Root file given its name
 *
 *  @param filename name of the file to open.
 *  @note Pointer to the file is saved in m_pFile.
 *  @note It's up to the caller to do any save/restore of the root current
 *        directory as this operation sets it to the newly opened file.
 */
void
RootTreeSink::openFile(const std::string& filename)
{
    m_pFile = new TFile(filename.c_str(), "UPDATE");   // Add to any existing file.
}
/**
 * createTree
 *    - Figure out the set of parameters to put in the tree.
 *    - Create and stock and appropriate SpecTclRootTree from those.
 *    - The pointer to the resulting, dynamically allocated SpecTclRootTree is
 *      saved in m_pTree.
 *
 *  @note - The m_parameterPatterns vector is a vector of glob patterns.  If
 *          any of them match a parameter name, that parameter is included
 *          in the tree.
 */
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
 * defaultFilename
 *    @return std::string - A default filename used if no begin run item
 *                          defines the TFile name.
 */
std::string
RootTreeSink::defaultFilename()
{
    std::stringstream sName;
    sName << "SpecTcl-" << m_nNumBeginSequence++ << ".root";
    return sName.str();
}
/**
 * tearDown
 *    If the file is open and the tree exists, flush everything out to file.
 *    destroy the tree and destroy the file object.
 */
void
RootTreeSink::tearDown()
{
    if (m_pFile) {
        m_pFile->Flush();
        delete m_pTree;
        delete m_pFile;
        
        m_pTree = nullptr;
        m_pFile = nullptr;
    }
}