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
#include "GateContainer.h"
#include "TclGrammerApp.h"
#include "TreeBuilder.h"
#include <Event.h>
#include <EventList.h>
#include <SpecTcl.h>
#include <TDirectory.h>
#include <TFile.h>
#include <stdio.h>

#include <fnmatch.h> // For parameter pattern matching.
#include <sstream>
#include <stdexcept>

// Let's define an exception class we'll use to signal root exceptions:
// (Issue #217)
class RootException : public std::runtime_error {
public:
  RootException(const std::string &what) : std::runtime_error(what) {}
  RootException(const char *what) : std::runtime_error(what) {}
  RootException(const RootException &other) : std::runtime_error(other) {}
  RootException &operator=(const RootException &other) {
    return static_cast<RootException &>(std::runtime_error::operator=(other));
  }
};

/**
 * constructor
 *   @param name     - name of the tree to create.
 *   @param patterns - Vector of parameter name patterns.
 *   @param gate     - Gate container of the condition under which items are
 *                      added to the tree.
 *   @note - We just initialize the data members as the tree gets built
 *           by OnBegin.
 */
RootTreeSink::RootTreeSink(std::string name,
                           const std::vector<std::string> &patterns,
                           CGateContainer *m_pGate)
    : m_pFile(0), m_pTree(0), m_Gate(*m_pGate), m_parameterPatterns(patterns),
      m_treeName(name), m_outputDir{}, m_enabled(false) // open will enable.
{}
/**
 * destructor.
 *    Write if the file is open and destroy all the dynamic bits and pieces.
 */
RootTreeSink::~RootTreeSink() { tearDown(); }

/**
 * OnOpen
 *    A new file has been opened for us.
 *    - Complain if we know the file is already open.
 *    - Create a new tree.
 *  @param pNewFile - the file that's been opened for our trees.
 */
void RootTreeSink::OnOpen(TFile *pNewFile) {
  CTclGrammerApp::getInstance()->enableRootErrors();
  // Not sure if I can sanely tear down the tree when the file has been
  // yanked out from underneath it, so:

  try {
    if (m_pFile) {
      std::cerr << "** Warning -- RootTreeSink::OnOpen - Root file has not "
                   "been closed \n";
      std::cerr << "**            This will result in a small memory leak\n";
    }
    m_pFile = pNewFile;
    createTree();
    enable(); // Now that we have a file we are enabled.
  } catch (...) {
    CTclGrammerApp::getInstance()->disableRootErrors();
    throw;
  }
  CTclGrammerApp::getInstance()->disableRootErrors();
}
/**
 * OnAboutToClose
 *     The guy managing the TFile* is about to close it.
 *     teardown our tree and null out the tree/file pointers.
 */
void RootTreeSink::OnAboutToClose() {

  tearDown(); // Nulls the pointers for us too.
}

/**
 * operator()
 *    Called when a set of events must be processed:
 *    - If no file/tree has beens set up, set it up.
 *    - Process each event in the list.
 *
 *  @param rEvents - events to process (basically a vector of event references).
 */
void RootTreeSink::operator()(CEventList &rEvents) {
  // Just silently ignore the data if we've not got a file.
  CTclGrammerApp::getInstance()->enableRootErrors();
  try {
    if (m_pFile && isEnabled()) {

      // Process the events one at a time.

      for (int i = 0; i < rEvents.size(); i++) {
        if (rEvents[i]) {
          (*this)(*(rEvents[i]));
        } else {
          break; // Three aren't any more events in the list.
        }
      }
    }
  } catch (RootException &e) {
    std::cerr << "Failure in root tree " << m_treeName
              << "  Tree will be disabled until next run begins: " << e.what()
              << std::endl;
    disable(); // Not returning here allows root errors to be diabled.
  } catch (...) {
    CTclGrammerApp::getInstance()->disableRootErrors();
    throw;
  }
  CTclGrammerApp::getInstance()->disableRootErrors();
}
/**
 *  OnBegin
 *    A begin run has happened.. if the file is open, we teardown the tree,
 * close the file then create a  new file and invoke OnOpen to do the rest of
 * the work.
 *   @param runNumber - number of the new run.
 *   @param title     - Title of the run Title s the root file.
 *
 * @note using UPDATE supports the case where we have multiple begin runs in a
 * source as we do for event built data.
 */
void RootTreeSink::OnBegin(unsigned runNumber, const char *title) {
  if (m_pFile) {
    auto pFile = m_pFile;
    OnAboutToClose();
    delete pFile;
  }
  // A new filename of the form run-nnnn.root:

  CTclGrammerApp::getInstance()->enableRootErrors();
  try {
    char filename[1000]; // More than enough I think:

    // Issue #246 - if the output directory is set, we need to prepend it to the
    // filename. We also check to make sure there's a '/' between the path and
    // the filename if the path doesn't end with one. If the output directory
    // is not set, we just create the file in the current directory as before.

    if (!m_outputDir.empty()) {
      std::string path = m_outputDir;
      if (path.back() != '/') {
        path += '/';
      }
      snprintf(filename, sizeof(filename), "%s%s-run-%04u.root", path.c_str(),
               m_treeName.c_str(), runNumber);
    } else {
      snprintf(filename, sizeof(filename), "%s-run-%04u.root",
               m_treeName.c_str(), runNumber);
    }
    std::string oldDir = gDirectory->GetPath();
    gDirectory->Cd("/");
    TFile *pFile = new TFile(filename, "UPDATE", title);
    if (!pFile) {
      std::stringstream errormsg;
      errormsg << "Failed to open root tree file : " << filename << std::endl;
      std::string msg(errormsg.str());
      throw RootException(msg);
    }
    OnOpen(pFile);
    gDirectory->Cd(oldDir.c_str());
    enable(); // Enable the tree.
  } catch (RootException &e) {
    std::cerr << "Root tree: " << m_treeName
              << " Failed on OnBegin: " << e.what()
              << "\nTree will be disabled until next begin run \n";
    disable(); // Not returning here allows root errors to be disabled.

  } catch (...) {
    CTclGrammerApp::getInstance()->disableRootErrors();
    throw;
  }
  CTclGrammerApp::getInstance()->disableRootErrors();
}
/**
 * OnEnd
 *   An end run was detected - the file, if opened is closed.
 *
 * @param runNumber - number of the run being closed.
 * @param title  - Title of the run.
 *
 * @note - In this implementation if there multiple END run items, the event
 * data that might be intermixed between them (if barrier synch is off) will be
 * lost from the output file.
 */
void RootTreeSink::OnEnd(unsigned runNumbver, const char *title) {
  if (m_pFile) {
    auto pFile = m_pFile;
    OnAboutToClose();
    Int_t status = pFile->Write();
    delete pFile;
    if (status < 0) {
      // Write filed but we still want to delete it

      std::stringstream errormsg;
      errormsg << " Failed the final flush of tree: " << m_treeName;
      std::string msg(errormsg.str());

      // don't throw because I don't think there's a catcher and, in any
      // event, the run is done.
    }
    disable(); // Disable until the next open.
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
void RootTreeSink::operator()(CEvent &event) {
  (m_Gate)->RecursiveReset(); // Clear gate and what it might depend on.
  if ((*m_Gate)(event)) {
    if (m_pTree->Fill(event) < 0) {
      // THe fill failed the operator() that does the event list will catch
      // the exception and diable the tree.  We don't need to do anything.
      // We won't get called if diabled either.

      std::stringstream errormsg;
      errormsg << "Tree fill failed for root tree: " << m_treeName << std::endl;
      std::string msg(errormsg.str());
      throw RootException(msg);
    }
  }
}
void RootTreeSink::createTree() {
  // Figure out which parameters match the patterns:

  SpecTcl *pApi = SpecTcl::getInstance();
  std::vector<ParameterTree::ParameterDef> params;
  for (auto p = pApi->BeginParameters(); p != pApi->EndParameters(); p++) {
    std::string name = p->first;
    unsigned id = p->second.getNumber();
    for (int i = 0; i < m_parameterPatterns.size(); i++) {
      if (!fnmatch(m_parameterPatterns[i].c_str(), name.c_str(), 0)) {
        // Match:
        params.push_back(ParameterTree::_ParameterDef(name.c_str(), id));
        break; // No more patterns to check.
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
void RootTreeSink::tearDown() {
  if (m_pFile) {
    CTclGrammerApp::getInstance()->enableRootErrors();
    try {
      // See the stuff below about why we don't just immediately
      // fail this function.

      auto writeStat = m_pFile->Write();
      m_pFile->Flush(); // void. :-(

      // We still want to tear crap down.. no need
      // for an exception since the we're done writing the tree.
      delete m_pTree;

      m_pTree = nullptr;
      m_pFile = nullptr;
      if (writeStat < 0) {
        std::cerr << "Finalization of root tree " << m_treeName << " failed\n";
      }
      disable(); // Next file open will re-enable.
    } catch (...) {
      CTclGrammerApp::getInstance()->disableRootErrors();
      throw;
    }
    CTclGrammerApp::getInstance()->disableRootErrors();
  }
}