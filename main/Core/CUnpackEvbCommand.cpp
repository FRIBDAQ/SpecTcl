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

/** @file:  CUnpackEvbCommand.cpp
 *  @brief: Implement command to create event processors that unpack event data.
 */

#include "CUnpackEvbCommand.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include "CPipelineManager.h"
#include "CEventBuilderEventProcessor.h"
#include "CPipelineEventProcessor.h"
#include <Exception.h>
#include <stdexcept>
#include <sstream>
#include <tcl.h>

/**
 * Constructor
 *    Register the command with the interpreter.
 *
 * @param interp - interpreter on which the command is registered.
 */
CUnpackEvbCommand::CUnpackEvbCommand(CTCLInterpreter& interp) :
    CTCLObjectProcessor(interp, "evbunpack", true)
{}

/**
 * destructor
 *    We can't own the event processors we made so we let then live.
 *    The map will autodestruct.
 */
CUnpackEvbCommand::~CUnpackEvbCommand() {}

/**
 * operator()
 *    Gets control when the interpreter encounters the command.
 *    We ensure we have a subcommand, pick it out and dispatch
 *    based on the command.
 *
 * @param interp - references the interpreter that's executing the command.
 * @param objv   - vector of commmand line parameters.  Note objv[0] is the
 *                 command itself.
 * @return int - TCL_OK on success, TCL_ERROR on failure.
 * @note Exceptions are used to simplify error handling and to centralize
 *       error management.
 */
int
CUnpackEvbCommand::operator()(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    try {
        bindAll(interp, objv);
        requireAtLeast(objv, 2, "Insufficient parameters");
        std::string subcommand = objv[1];
        
        if (subcommand == "create") {
            create(interp, objv);
        } else if (subcommand == "addprocessor") {
            addprocessor(interp, objv);
        } else if (subcommand == "list") {
            list(interp, objv);
        } else {
            Usage(interp, objv, "Invalid subcommand keyword");
        }
    }
    catch(std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (CException& e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    }
    catch(...) {
        interp.setResult("Unanticipated exception type in evbunpack");
        return TCL_ERROR;
    }
    return TCL_OK;                     // Success is a drop through to here.
}
/**
 * create
 *    Creates a new unpacker for event built data.
 *    Command requires the following parameters:
 *    -  Name (string) of the processor.  This must not be an event processor
 *       that has been registered with the pipeline manager.
 *    -  mhz - double - Speed of the timestamp clock in megaherz.
 *    - basename (string) - Base name for the diagnostic parameters produced
 *              by this event processor.
 *
 * @param interp   - references the interpreter that's executing the command.
 * @param objv     - Vector of command line parameters.
 * @throw std::logic_error, std::string.
 */
void
CUnpackEvbCommand::create(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 5, "Incorrect number of parameters for create");
    
    std::string name = objv[2];
    double      mhz  = objv[3];
    std::string basename = objv[4];
    
    // Create the event processor.  This is done inside a try/catch
    // block so that we can release storage in case of error:
    
    CEventBuilderEventProcessor* pProcessor(0);
    try {
        pProcessor = new CEventBuilderEventProcessor(mhz, basename);
        CPipelineManager* pMgr = CPipelineManager::getInstance();
        pMgr->registerEventProcessor(name, pProcessor);  // throws on dup name.
        m_Processors[name] = pProcessor;
        
    }
    catch (...) {
        delete pProcessor;
        throw;               // Let upper levels actually deal with the error.
    }
}
/**
 * addprocessor
 *   Adds a pipeline as an event processor to the handle event fragments
 *   with a specific source id.  Command parameters are:
 *   - name (string) - Name ofthe event processor we're working on.  Must have been
 *            created with evbunpack create as those are the only ones
 *            we'll manipulate.
 *   - sid (integer) - Source id we're specifying a handler for.
 *   - pipe-name -name of the pipeline that will handle the processing.
 *
 *   Note that this implies that, on success, we'll create a
 *   CPipelineEventProcessor to wrap the pipeline and install that as
 *   the event processor for the CEventBuilderEventProcessor in question.
 *
 * @param interp - refernces the interpreter that's executing the command.
 * @param objv   - Vector of command parameters.
 */
void
CUnpackEvbCommand::addprocessor(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 5, "Missing parameters in addprocessor subcommand");
    std::string prcName = objv[2];
    int         sid     = objv[3];
    std::string pname   = objv[4];
    
    // Ensure that prcName is an event processor we manage else throw.
    
    ProcessorMap::iterator pProcessor = m_Processors.find(prcName);
    if (pProcessor == m_Processors.end()) {
        std::string msg = prcName;
        msg += " is not an event built data unpacker we're managing";
        throw msg;
    }
    CEventBuilderEventProcessor* pProcObject = pProcessor->second;
    
    // Get a pointer to the event pipeline or throw.
    
    CPipelineManager* pMgr = CPipelineManager::getInstance();
    CTclAnalyzer::EventProcessingPipeline* pPipe(0);
    for(auto p = pMgr->pipelineBegin(); p != pMgr->pipelineEnd(); p++) {
        if (p->first == pname) {
            pPipe = p->second;
            break;
        }
    }
    if (!pPipe) {
        std::string msg = pname;
        msg += " is not an event processing pipeline name";
        throw msg;
    }
    
    // Ensure our event processor doesn't already have a processor registered
    // for this sid.
    
    if(pProcObject->haveEventProcessor(sid)) {
        std::stringstream msg;
        msg << prcName
            << " already has an event processor defined for source ID: "
            << sid;
        throw msg.str();
    }
    
    // Wrap the pipeline in an event processor and add it to the
    // event built data event processor as requested.
    
    CPipelineEventProcessor* pPipeProcessor = new CPipelineEventProcessor(pPipe);
    pProcObject->addEventProcessor(sid, *pPipeProcessor);
}
/**
 * list
 *    Return a TCL list of the event processors we are managing.  Note there
 *    may be others, for example, the user may have programmatically
 *    created one at C++ level and registered it with the pipeline manager.
 *    Such a processor will be invisible to us and must be managed
 *    by the user's C++ code.
 *
 *    An optional glob pattern can restrict the list to those event processors
 *    that match that pattern.  The pattern defaults to * which matches all
 *    processors.
 *
 *  @param interp - Reference to the interpreter that's executing this command.
 *  @param objv   - Reference to the vector of command line parameters.
 *
 * Sets the result to a list of the matching parameters.
 */
void
CUnpackEvbCommand::list(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    std::string pattern = "*";                // Default match pattern.
    requireAtMost(objv, 3, "Too many command line parameters");
    if (objv.size() == 3) {
        pattern = std::string(objv[2]);       // User provided a pattern.
    }
    
    CTCLObject result;
    result.Bind(interp);
    
    for (auto p = m_Processors.begin(); p != m_Processors.end(); p++) {
        if (Tcl_StringMatch(p->first.c_str(), pattern.c_str())) {
            result += p->first;
        }
    }
    
    interp.setResult(result);
}
///////////////////////////////////////////////////////////////////////////////
//   Utilities:

/**
 * Usage:
 *   @param interp - interpreter running the command.
 *   @param objv   - Command line parameters.  objv[0] is the command.
 *   @param msg    - Error message to put at the top of the usage.
 *   
 */
void
CUnpackEvbCommand::Usage(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv, std::string msg
)
{
    std::stringstream smsg;
    std::string cmd = objv[0];
    
    smsg <<  msg << std::endl;
    smsg << "Usage\n";
    smsg << "  " << cmd << " create name mhz basename\n";
    smsg << "  " << cmd << " addprocessor name sid pipe-name\n";
    smsg << "  " << cmd << " list ?pattern?\n";
    smsg << "Subcommands:\n";
    smsg << "   create - creates a new processor:\n";
    smsg << "      name - is the name under which to register the processor\n";
    smsg << "      mhz  - is the timestamp clockspeed in MHz\n";
    smsg << "      basename - is the base name for the diagnostic parameters \n";
    smsg << "                 maintained by the processor\n";
    smsg << "   addprocessor - adds a processing pipeline to handle fragments\n";
    smsg << "                  from a specific source id where: \n";
    smsg << "       name     - Is the name of an event processor we created\n";
    smsg << "       sid      - Is the source id for which a pipe is being defined\n";
    smsg << "       pipe-nme - Is the event analysis pipeline to handle fragments\n";
    smsg << "                  from this source id.\n";
    smsg << "       list  - Lists the event processors we're managing\n";
    smsg << "          pattern - is an optional glob pattern that restricts \n";
    smsg << "                    the list to the processors whose name matches\n";
    smsg << "                    the pattern.  pattern defaults to * matching all strings\n";
    
    throw smsg.str();
}