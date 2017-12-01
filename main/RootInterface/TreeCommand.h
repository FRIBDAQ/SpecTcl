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

/** @file:  TreeCommand.h
 *  @brief: Tcl command extension to create tree event sinks.
 */
#ifndef TREECOMMAND_H
#define TREECOMMAND_H
#include <TCLObjectProcessor.h>

class CTCLInterpreter;
class CTCLObject;
class RootEventProcessor;


/**
 * @class TreeCommand
 *     Provides the roottree command.  This has the following forms:
 *
 *  \verbatim
 *  
 *     roottree create tree-name parameter-pattern-list ?gate?
 *     roottree delete tree-name
 *     roottree list ?tree-name-pattern?
 *     
 *  \verbatim
 *
 *  These are pretty self explanatory, however we need to say something about
 *  what the 'list' subcommand returns.  Naturally it'll return a Tcl list.
 *  Each list element is a sublist containing in order:
 *
 *  -  Name of the tree.
 *  -  Parameter pattern list
 *  -  Name of the gate that determines which events are written.  Note that
 *     for 'ungated' trees this will be "-T-"
 */
class TreeCommand : public CTCLObjectProcessor
{
private:
    RootEventProcessor*   m_pEventProcessor;
public:
    TreeCommand(CTCLInterpreter& interp, const char* cmdName);
    virtual ~TreeCommand();
    
    virtual int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
private:
    void create(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void destroy(CTCLInterpreter& interp, std::vector<CTCLObject>& objv); // delete is reserved.
    void list(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    
    RootEventProcessor* eventProcessor();
    std::string sinkName(std::string treeName) const;
};
#endif