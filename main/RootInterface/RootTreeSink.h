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

/** @file:  RootTreeSink.h
 *  @brief: CEVentSink that creates root trees.
 */
#ifndef ROOTTREESINK_H
#define ROOTTREESINK_H
#include <EventSink.h>
#include <vector>
#include <string>

class TFile;
class SpecTclRootTree;
class CGateContainer;
class CEvent;
class CEventList;

/**
 * @class RootTreeSink
 *    This class provides a CEventSink class that writes root trees.
 *    The object is provided with a list of parameter glob search masks.
 *    On begin run, the hierarchy of matching parametrs is turned into
 *    a similar hiearchy of tree branches in a SpecTclRootTree.
 *
 *    The object also has a gate container that must be satisfied if
 *    the tree is to have a set of leaves added to it (this could be a
 *    true gate to write an unfiltered tree.
 *
 *    On End run, the tree is flushed and the file is closed.
 *    The tree is also deconstructed.  This is done in case the user
 *    defines new parameters (either by registering additional event processors
 *    or by creating a scripted pseudo) between runs....thus the tree is
 *    constructed from the parameter list/mask each time the file is opened.
 *
 *    If data are received when a run is not open, a default file is used
 *    that is constructed from a base name and a serial number of the number
 *    of times we had to make this sort of file.
 *
 *    @note - File opens etc and tree construction are done in a way that
 *            preserves the root 'current working directory' so that nothing
 *            external to us is harmed.
 */

class RootTreeSink : public CEventSink
{
private:
    TFile*           m_pFile;
    SpecTclRootTree* m_pTree;
    CGateContainer&  m_Gate;
    std::vector<std::string> m_parameterPatterns;
    unsigned         m_nNumBeginSequence;
    std::string      m_treeName;
public:
    RootTreeSink(std::string name, const std::vector<std::string>& patterns, CGateContainer* m_pGate);
    virtual ~RootTreeSink();                       // not sure if final so...
    
    
    void OnBegin(unsigned run, std::string title);
    void OnEnd();
    virtual void operator()(CEventList& rEvents);
private:
    void operator()(CEvent& rEvent);
    void openFile(const std::string& filename);
    void closeFile();
    void createTree();
    std::string defaultFilename();
    void tearDown();
    
};

#endif