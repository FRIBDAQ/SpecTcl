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

/** @file:  TreeCommand.cpp
 *  @brief: Implement the roottree command.
 */
#include <TreeCommand.h>
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <SpecTcl.h>
#include <Exception.h>
#include <stdexcept>
#include <Spectrum.h>
#include <GateContainer.h>

#include "RootEventProcessor.h"
#include "RootTreeSink.h"


/**
 * constructor
 *    Just register command and initialize m_pEventProcessor.
 *
 *  @param interp - interpreter on which the commnand is registered.
 *  @param cmdName - Command name string.
 */
TreeCommand::TreeCommand(CTCLInterpreter& interp, const char* cmdName) :
    CTCLObjectProcessor(interp, cmdName, true),
    m_pEventProcessor(0)
{}

/**
 * destructor
 *    If the event processor exists remove it and destroy it.
 */
TreeCommand::~TreeCommand()
{
    if (m_pEventProcessor) {
        SpecTcl* api = SpecTcl::getInstance();
        CTclAnalyzer::EventProcessorIterator p =
            api->FindEventProcessor(*m_pEventProcessor);
            
        // Should be found:
        
        if (p != api->ProcessingPipelineEnd()) {
            api->RemoveEventProcessor(p);    
        }
        delete m_pEventProcessor;
    }
}
/**
 *  operator()
 *     Called when the command has been issued.
 *     - Bind the objects to the interpreter.
 *     - Ensure there's a subcommand.
 *     - Dispatch to the appropriate handler.
 *     
 * @note we use exception processing for error handling
 * @param interp - The interpreter executing the command.
 * @param objv   - Vector of command words.
 * @return int   - TCL_OK - command successful.
 *                 TCL_ERROR - Command failed.  The result is an error message
 *                             that is user readable.
 */
int
TreeCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    bindAll(interp, objv);
    try {
        requireAtLeast(objv, 2, "roottree - needs a subcommand");
        
        std::string subcommand = objv[1];
        if (subcommand == "create") {
            create(interp, objv);
        } else if (subcommand == "delete") {
            destroy(interp, objv);
        } else if (subcommand == "list") {
            list(interp, objv);
        } else {
            std::string msg = "subcommand ";
            msg += subcommand;
            msg + " is not a valid subcommand";
            throw msg;
        }
    }
    catch (CException& e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    }
    catch (const char* msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("roottree command caught an unexpected exception type");
        return TCL_ERROR;
    }
    
    return TCL_OK;
}
/**
 * create
 *    Creates a new event sink for root gating and hooks it into the
 *    system.
 *    -  Ensure we have sufficient command parameters.
 *    -  Look up the gate and get its container.
 *    -  Produce a warning message if none of the parameter patterns
 *       match anything now.
 *    -  Get the event processor (creates/registers if needed),.
 *    -  Create the sink.
 *    -  Add the sink as a sink.
 *    -  Add the sink to the event processor.
 *
 *    Parameters are as for operator().  Errors are reported via exceptions.
 */
void
TreeCommand::create(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    SpecTcl* pApi = SpecTcl::getInstance();
    
    requireAtLeast(objv, 4, "roottree create requires at least a name and parameter pattern list");
    requireAtMost(objv, 5, "roottree create can at most have a name, parameter patternlist and gate name");
 
    CGateContainer* pGate = pDefaultGate;           // Use default gate if none supplied.
    
    
    // Get the treename and save the list of patterns in a vector.
    
    std::string treeName = objv[2];
    std::vector<std::string> patterns;
    for (int i = 0; i < objv[3].llength(); i++) {
        patterns.push_back(std::string(objv[3].lindex(i)));
    }
    // If a gate name has been defined, look it up:
    
    if (objv.size() == 5) {
        std::string gateName = objv[4];
        pGate = pApi->FindGate(gateName);
        if (!pGate) {
            std::string message = "roottree create - No gate named  ";
            message += gateName;
            message += " has been defined";
            throw message;
        }
    }
    // If there are no matches for any of the patterns with the parameters,
    // set a result with a warning about that:
    
    bool foundOne(false);
    for (auto p = pApi->BeginParameters(); (p != pApi->EndParameters()) && !foundOne; p++)  {
        std::string paramName = p->first;
        for (int i = 0; i < patterns.size(); i++) {
            if (Tcl_StringMatch(paramName.c_str(), patterns[i].c_str())) {
                foundOne = true;
                break;
            }
        }
    }
    // foundOne means at least one match was found;
    
    if(!foundOne) {
        interp.setResult("*Warning* - parameter patterns don't match any parameters");
    }
    // Now we can start creating things and hooking them in:
    
    RootTreeSink* pSink = new RootTreeSink(treeName, patterns, pGate);
    RootEventProcessor *pEp = eventProcessor();   // Creates/registers if needed.
    
    std::string sinkName("root-tree:");
    sinkName += treeName;
    pApi->AddEventSink(*pSink, sinkName.c_str());
    try {
        pEp->addTreeSink(treeName.c_str(), pSink);
    }
    catch (...) {
        // Add failed,, remove and delete:
        
        pApi->RemoveEventSink(sinkName);
        delete pSink;
        throw;                         // This is an error.
    }
    // Falling here is success.
}
/**
 * destroy (delete is a reserved word).
 *
 *   Parameters are as for operator() and errors are reported by throwing
 *   exceptions.
 */
void
TreeCommand::destroy(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3, "roottree delete requirea a tree name");
    std::string treeName = objv[2];
    
    RootEventProcessor* pProcessor = eventProcessor(); // creates if user is perverse.
    RootTreeSink*       pSink      = pProcessor->removeTreeSink(treeName.c_str());
    
    // The removeTreeSink method threw if there was no such sink.
    
    SpecTcl* pApi = SpecTcl::getInstance();
    pApi->RemoveEventSink(treeName);
    delete pSink;
    
}
/**
 * list
 *    List the tree sinks and their characteristics.  The result
 *    is set to a list containing one description entry for each tree that
 *    matches the pattern (default pattern is * which matches everything).
 *    The descriptions are a three element list containing in order:
 *    -   Name of the tree.
 *    -   List of parameter patterns for parameters to be written to the tree.
 *    -   Name of the gate that filters events written to the tree.
 *
 * Parameters are as for operator()
 * Errors are flagged by throwing an exception.
 */
void
TreeCommand::list(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireAtMost(objv, 3, "roottree list - can have at most a tree name pattern");
    
    std::string pattern = "*";              // Default pattern.
    if (objv.size() ==3) {
        pattern = std::string(objv[2]);                 // if user provided one.
    }
    RootEventProcessor* pProcessor = eventProcessor();
    
    CTCLObject          result;
    result.Bind(interp);
    
    for(auto p =  pProcessor->begin(); p != pProcessor->end(); p++)
    {
        CTCLObject entry;         entry.Bind(interp);
        CTCLObject parameterList; parameterList.Bind(interp);
        
        std::string name = p->first;
        if (Tcl_StringMatch(pattern.c_str(), name.c_str())) {
            const std::vector<std::string>& patterns(p->second->getParameterPatterns());
            CGateContainer& gc (p->second->getGate());
            std::string gateName = gc.getName();
            
            // Build list of patterns.
            
            for (int i = 0; i < patterns.size(); i++) {
                parameterList += patterns[i];
            }
            // build entry  in result list:
            
            entry += name;
            entry += parameterList;
            entry += gateName;
            
            // Add it to the list.
            
            result += entry;
        }
    }
    
    
    interp.setResult(result);
}
/**
 * eventProcessor
 *    - If the event processor does not yet exist (m_pEventProcessor is null)
 *      it's created and hooked in.  Note that since event sinks run after
 *      all event processor pipeline elements, it's not necessary to
 *      put this event processor anywhere in particular in the pipeline.
 *      All it does is inform sinks of run state transitions.
 *    - Return m_pEventProcessor.
 *
 * @return RootEventProcessor*
 */
RootEventProcessor*
TreeCommand::eventProcessor()
{
    if (!m_pEventProcessor) {
        m_pEventProcessor = new RootEventProcessor;
        SpecTcl* pApi = SpecTcl::getInstance();
        pApi->AddEventProcessor(*m_pEventProcessor, "RootTreeEventProcessor");
    }
    return m_pEventProcessor;
}