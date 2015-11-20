/**

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2013.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#            Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321

##
# @file   SpectrumStatsCommand.cpp
# @brief  Implementation of CSpectrumStatsCommand
# @author <fox@nscl.msu.edu>
*/
#include "CSpectrumStatsCommand.h"
#include <SpecTcl.h>
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <tcl.h>
#include <stdexcept>

/**
 * constructor
 *
 * @param interp - interpreter on wich the command will be registered.
 * @param command - word that will execute the command.
 */
CSpectrumStatsCommand::CSpectrumStatsCommand(
    CTCLInterpreter& interp, const char* command
) :
    CTCLObjectProcessor(interp, command, true)
{}
/**
 * destructor
 */
CSpectrumStatsCommand::~CSpectrumStatsCommand()
{}

/**
 * operator()
 *    Executes the command.  For each matching spectrum produces
 *    a dict and adds it to the list of dicts.
 * @param interp - Reference to encapsulated Tcl interpreter object
 * @param objv   - Reference to the vector of encapsulated Tcl_Obj's that make up the
 *                 command word.
 *
 * @return int
 * @retval TCL_OK - Successful completion.
 * @retval TCL_ERROR - Failure.
*/
int CSpectrumStatsCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    bindAll(interp, objv);
    
    // We use exception handling to deal with errors:
    
    try {
        // There can be at most two command words (command, pattern).
        // Verify this and figure out the match pattern.
        
        std::string pattern("*");       // Default.
        requireAtMost(objv, 2, Usage(std::string(objv[0])).c_str());
        if (objv.size() == 2) {
            pattern = std::string(objv[1]);
        }
        
        CTCLObject result;
        result.Bind(interp);             // Could be an empty lsit.
        
        // Iterate over the spectra:
        
        SpecTcl* pApi = SpecTcl::getInstance();
        SpectrumDictionaryIterator p = pApi->SpectrumBegin();
        while (p != pApi->SpectrumEnd()) {
            std::string name = p->first;
            if(Tcl_StringMatch(name.c_str(), pattern.c_str())) {
                CTCLObject* pDict = makeStatsDict(interp, name, p->second);
                result += *pDict;
                delete pDict;
            }
            p++;
        }
        
        interp.setResult(result);
    }
    catch(std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    }
    return TCL_OK;
  
}

/*----------------------------------------------------------------------------
 * private utilities:
 */

/**
 * Usage
 *    Provides a string that describes how to use this command.
 *  @param command - string of the command name.
 *  @return std::string - Command usage:
 */
std::string
CSpectrumStatsCommand::Usage(std::string command)
{
    std::string message = "Usage\n   ";
    message += command;
    message += " ?pattern?";
    
    return message;
}
/**
 * makeStatsDict
 *    Make a statistics dictionary for a spectrum. See the class comment
 *    for the keys that are in this dict.
 *
 *    @param interp - Interpreter to which objects will be bound.
 *    @param name   - Spectrum name.
 *    @param pSpec  - Pointer to the spectrum
 *    @return CTCLObject* - Pointer to a dynamically allocated return object.
 *    @note - a dict has a list rep which consists of [list key1 val1 key2 val2...]
 *            We're actually going to build that since it's simpler.
 *            This will shimmer into a dict on the first dict-like usage.
 *            such is Tcl.
 */
CTCLObject*
CSpectrumStatsCommand::makeStatsDict(
    CTCLInterpreter& interp, std::string name, CSpectrum* pSpectrum
)
{
    CTCLObject* pObj = new CTCLObject;
    pObj->Bind(interp);
    
    (*pObj) += "name";
    (*pObj) += pSpectrum->getName();
    
    // Underflows
    
    (*pObj) += "underflows";
    std::vector<unsigned> uflows = pSpectrum->getUnderflows();
    CTCLObject underflows;
    underflows.Bind(interp);
    for (int i = 0; i < uflows.size(); i++) {
        underflows += (int)uflows[i];
    }
    (*pObj) += underflows;
    
    // Overflows
    
    (*pObj) += "overflows";
    uflows = pSpectrum->getOverflows();
    CTCLObject overflows;
    overflows.Bind(interp);
    
    for (int i =0; i < uflows.size(); i++) {
        overflows += (int)uflows[i];
    }
    (*pObj) += overflows;
    
    // Return the result.
    
    return pObj;
}
