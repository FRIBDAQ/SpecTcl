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

/** @file:  CDBCommand.cpp
 *  @brief: Implements the command ensemble to control database event operations.
 */

#include "CDBCommands.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include "DBEvents.h"
#include "CDBEvents.h"
#include <stdexcept>
#include <Exception.h>
#include <sstream>
#include <SpecTcl.h>

/**
 * construtor - all the heavy lifting is done by the base class:
 *
 *  @param interp - references the interpreter that will run this command.
 *  @param name   - The optional base name of the command (defaults to daqdb).
 */
CDBCommands::CDBCommands(CTCLInterpreter& interp, const char* name) :
    CTCLObjectProcessor(interp, name, true),
    m_pWriter(nullptr),
    m_pEventProcessor(nullptr),
    m_enabled(false),
    m_nProcessorIndex(0)
{}

/**
 * destructor
 *    Destroy the event processor and the writer.  The event processor
 *    destructor is smart enough to remove it from the event processing
 *    pipeline.
 */
CDBCommands::~CDBCommands()
{
    delete m_pEventProcessor;        // delete of nullptr is allowed and a no-op.
    delete m_pWriter;
}

/**
 * operator()
 *    Just ensure we have a subcommand and dispatch based on it.
 *    We also set up exeption handling so that errors can be reported vi
 *    exceptions in the subcommand processors.  Futhermore we bind all the
 *    parameters to the interpreter.
 *
 *   @param interp - references the interpreter running this object.
 *   @param objv   - references a vector of encapsulated command line words.
 *   @return int   - TCL_OK on success, TCL_ERROR otherwise.
 */
int
CDBCommands::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    bindAll(interp, objv);
    try {
        requireAtLeast(objv, 2, "We need at least a subcommand");
        
        std::string sub = objv[1];        // Sub command.
        if (sub == "open") {
            dbOpen(interp, objv);
        } else if (sub == "enable") {
            dbEnable(interp, objv);
        } else if (sub == "disable") {
            dbDisable(interp, objv);
        } else if (sub == "close") {
            dbClose(interp, objv);
        } else if (sub == "autosave") {
            dbAutoSave(interp, objv);
        } else {
            std::stringstream msg;
            msg << "Invalid subcommand: '" << sub <<  "'";
            throw std::invalid_argument(msg.str());
        }
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    }
    catch (CException& e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    }
    catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (const char* msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    // Fall through without an exception means success.
    
    return TCL_OK;
}
//////////////////////////////////////////////////////////////////////////////
// Subcommand processors.

/**
 * dbOpen
 *   - Create a new writer.
 *   - If there's no existing processor create it as well.
 *   - If there is, just set its writer.
 *   @param interp - references the interpreter running this object.
 *   @param objv   - references a vector of encapsulated command line words.
 */
void
CDBCommands::dbOpen(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    // need a filename:
    
    requireExactly(objv, 3, "open subcommand requires a filename (only)");
    
    std::string filePath = objv[2];
    
    CDBEventWriter* pWriter = new CDBEventWriter(filePath.c_str());
    
    if (!m_pEventProcessor) {            // Not setup yet.
        m_pWriter = pWriter;
        m_pEventProcessor = new CDBProcessor(pWriter);
        SpecTcl* pApi = SpecTcl::getInstance();
        pApi->AddEventProcessor(*m_pEventProcessor, processorName().c_str());
    } else {                            // Already setup.
        m_pEventProcessor->setWriter(pWriter);
        delete m_pWriter;
        m_pWriter = pWriter;
    }
    m_enabled = false;           // Open turns off enable.
}
/**
 * dbEnable
 *    Enable processing
 *    - We must be open.
 *    - We should not already be enabled.
 *
 *   @param interp - references the interpreter running this object.
 *   @param objv   - references a vector of encapsulated command line words.
 */
void
CDBCommands::dbEnable(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    // No subcommands.
    
    requireExactly(
        objv, 2, "The enable subcommand does not have any more parameters"
    );
    requireOpen();              // Ensures there is an event processor too.
    requireDisabled();          // prevents double enables.
    
    m_pEventProcessor->enable();
    m_enabled = true;
}
/**
 * dbDisable
 *    Disable event processing.
 *    - We must be open.
 *    - We must be enabled.
 *
 *   @param interp - references the interpreter running this object.
 *   @param objv   - references a vector of encapsulated command line words.
 */
void
CDBCommands::dbDisable(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(
        objv, 2, "The disable subcommand does not have any more parameters"
    );
    
    requireOpen();
    requireEnabled();
    
    m_pEventProcessor->disable();
    m_enabled = false;
}
/**
 * dbClose
 *    - Must be open.
 *    - Must be disabled.
 *    - Tears down the entire structure of the world.
 *   @param interp - references the interpreter running this object.
 *   @param objv   - references a vector of encapsulated command line words. 
 */
void
CDBCommands::dbClose(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(
        objv, 2, "The close subcommand does not have any more parameters"
    );
    requireOpen();
    requireDisabled();
    
    SpecTcl* pApi = SpecTcl::getInstance();
    auto p        = pApi->FindEventProcessor(*m_pEventProcessor);
    if (p == pApi->ProcessingPipelineEnd()) {
        throw std::logic_error(
            "BugCheck - dbclose asked to remove a nonexistent event processor"
        );
    }
    pApi->RemoveEventProcessor(p);
    
    //Deleting the procesor is not safe as it remains registered with the
    //pipeline manager. TODO: provide safe mechanism to delete event
    // processor registrations (note must not be in any pipeline).
    // For now -- leak
    //delete m_pEventProcessor;
    //delete m_pWriter;
    
    m_pEventProcessor = nullptr;
    m_pWriter         = nullptr;
}
/**
 * dbAutoSave
 *    Set the auto save list.
 *    Note that this is legal at any time.  If done while analyzing the run,
 *    the specified spectra will be saved when the end run is seen.
 * @param interp - interpretr running the command.
 * @param objv   - Vector of encapsulated command words.
 */
void
CDBCommands::dbAutoSave(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireOpen();                  // Else there's not an event processor etc.
    requireExactly(objv, 3, "daqdb autosave needs list of spectra (TclList)");
    std::vector<CTCLObject> nameList = objv[2].getListElements();
    
    SpecTcl* api = SpecTcl::getInstance();
    std::vector<std::string> spectrumNames;
    for (int i = 0; i < nameList.size(); i++) {
        std::string specName = nameList[i];
        if (api->FindSpectrum(specName)) {
            spectrumNames.push_back(specName);
        } else {
            std::stringstream msg;
            msg << specName << " is not a valid spectrum name";
            throw std::invalid_argument(msg.str());
        }
    }
    m_pWriter->setAutoSaveSpectra(spectrumNames);
    
}
///////////////////////////////////////////////////////////////////////////////
// Private utility methods.

/**
 * requireOpen
 *    If there's no open database throw a logic error.
 */
void
CDBCommands::requireOpen()
{
    if  ((!m_pWriter) || (!m_pEventProcessor)) {
        throw std::logic_error("daqdb - there is no open database file");
    }
}
/**
 * requireEnabled
 *    throw if the databsae is not enabled.
 */
void
CDBCommands::requireEnabled()
{
    if (!m_enabled) {
        throw std::logic_error("daqdb - database writing must be enabled");
    }
}
/**
 * requireDisabled
 *   throw if the database is enabled.
 */
void
CDBCommands::requireDisabled()
{
    if (m_enabled) {
        throw std::logic_error("daqdb - database writing must be disabled");
    }
}
/**
 * processorName
 *    Make a unique event processor processor name.
 *  @return std::string
 */
std::string
CDBCommands::processorName()
{
    std::stringstream s;
    s << "SpecTcl-sqlite3_" << m_nProcessorIndex++;
    return s.str();
}