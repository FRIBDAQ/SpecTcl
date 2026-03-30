/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2026
    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
/** 
 * @file CTreeVecCommand.cpp
 * @brief implement5 the "treeparamvec" command class to query about CTreeParameterVector objects.
 * @author Ron Fox <fox at frib dot msu dot edu>
 */

#include "CTreeVecCommand.h"
#include "TreeParameter.h"

#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <TclDict.h>
#include <stdexcept>
#include <Exception.h>
#include <sstream>
#include <tcl.h>



/**
 * constuctor
 *    Constructs our implementation command which is namespaced as 
 * [spectcl::serial::treeparamvec] 
 * 
 * @param rInterp - the interpreter running the command.
 * 
 */
CTreeVecActual::CTreeVecActual(CTCLInterpreter& rInterp) :
    CTCLObjectProcessor(rInterp, "spectcl::serial::treeparamvec", TCLPLUS::kfTRUE)
{}

/**
 * destructor
 * 
 */
CTreeVecActual::~CTreeVecActual() {}

/**
 *  operator()]
 *     Dispacteste subcomman to the appropriate processor.
 * 
 * @param interp - references the interpreter running the command.
 * @param objv   - the command words.
 * @return int - TCL_OK on success. The result may or may not be set.
 */
 int
 CTreeVecActual::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    try {
        bindAll(interp, objv);                                                         // bind all objects.
        requireAtLeast(objv, 2, "treeparamvec needs at least a subcommand option");    // ensure we have a subcommand.
        

        std::string subCommand = objv[1]; 

        // Dispatch based on subcommand:

        if (subCommand == "-list") {
            list(interp, objv);
        } else if (subCommand == "-setlow") {
            setlow(interp, objv);
        } else if (subCommand == "-sethigh") {
            sethigh(interp, objv);
        } else if (subCommand == "-setunits") {
            setunits(interp, objv);
        } else {
            std::stringstream strMsg;
            strMsg << "Invalid subcommand option: " << subCommand;
            std::string msg(strMsg.str());
            throw msg;
        }
    }
    catch (std::string msg) {
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

    return TCL_OK;
}

 ////////////////////////// Protecte subcommand handlers.

 /**
  * list
  *    This (nearly) always succeeds.  Sets the result to a list of tree parameter vector
  * descriptions whos name that match the optional pattern.  If the pattern is omitted, 
  * it defaults to * listing all tree parameter vectors.
  * 
  * @param interp - the interpreter executing this command.
  * @param objv   - the vector of command words.
  * @throw std::string if there are too many command line parameters.
  */
 void
 CTreeVecActual::list(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    // There can be at most a pattern:

    requireAtMost(objv, 3, "There can be at most, just one pattern parameter");

    // Figure out the match pattern:

    std::string pattern = "*";                // Default pattern.
    if (objv.size() == 3) {
        pattern = std::string(objv[2]);
    }

    CTCLObject result;
    result.Bind(interp);

    // Iterate over the tree parameter vectors:

    auto p = CTreeParameterVector::begin();
    while (p != CTreeParameterVector::end()) {
        // Simplest thing to do with matches is construct a new vector and then destroy it:

        if (Tcl_StringMatch(p->first.c_str(), pattern.c_str())) {
            result += describeVector(interp, p->first, p->second);
        }
        ++p;
    }

    interp.setResult(result);
 }
/**
 * setlow 
 *    Set the low limit of a tree parameter vector.  This affects the new parameters created as well
 * as all existing parameters.
 * 
 * @param interp - references the interpreter running the command.
 * @param objv   - the command words.  A vector name is required and a value that decodes as floating.
 * @throw CTreeParameterVector::NoSuchVectorException   if the named vector does not exist.
 * @throw std::string wrong number of parameters.
 * @note CTreeParameterVector::NoSuchVectorException  is derived from std::exception so the caller's
 * catch block will handle it.
 * @note no value is set in the interpreter result for this subcommand.
 */
void
CTreeVecActual::setlow(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    requireExactly(objv, 4, "-setlow requires a tree vector name and a new low limit.");

    std::string name = objv[2];
    double    newlow = objv[3];
    CTreeParameterVector vec = CTreeParameterVector::find(name.c_str());  // Can throw.
    vec.setLow(newlow);
}
/**
 *  sethigh
 *     Set a new value for the high limit of a parameter vector.  This sets the new high
 * value for, not only new parameters created, but for any existing parameters already
 * created.
 * 
 * @param interp - references the interpreter running the command.
 * @param objv   - the command words.  A vector name is required and a value that decodes as floating.
 * @throw CTreeParameterVector::NoSuchVectorException   if the named vector does not exist.
 * @throw std::string wrong number of parameters.
 * @note CTreeParameterVector::NoSuchVectorException  is derived from std::exception so the caller's
 * catch block will handle it.
 * @note no value is set in the interpreter result for this subcommand.
 */
void
CTreeVecActual::sethigh(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    requireExactly(objv, 4, "-setlow requires a tree vector name and a new low limit.");

    std::string name = objv[2];
    double    newhi = objv[3];
    CTreeParameterVector vec = CTreeParameterVector::find(name.c_str());  // Can throw.
    vec.setHigh(newhi);
}

/** setunits
 *    SEts a new units value for a vectorr.   Again, this sets both the units for any new parameters
 * created as well as the value for existing parameters.
 * 
 @param interp - references the interpreter running the command.
 * @param objv   - the command words.  A vector name is required and a new units value.
 * @throw CTreeParameterVector::NoSuchVectorException   if the named vector does not exist.
 * @throw std::string wrong number of parameters.
 * @note CTreeParameterVector::NoSuchVectorException  is derived from std::exception so the caller's
 * catch block will handle it.
 * @note no value is set in the interpreter result for this subcommand.
 */
void
CTreeVecActual::setunits(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    requireExactly(objv, 4, "-setlow requires a tree vector name and a new low limit.");

    std::string name = objv[2];
    std::string newunits = objv[3];

    CTreeParameterVector vec = CTreeParameterVector::find(name.c_str());  // Can throw.
    vec.setUnits(newunits.c_str());
}
 ///////////////////////////////////////////////// private utility methods.

 /**
  * describeVector - describe a tree parameter vector
  * 
  * @param interp - the interpreter executing the -list operation.
  * @param name   - the name of the vector.
  * @param pInfo  - pointer to the parameter info block.
  * @return CTCLObject - containing the dict describing the vector.. See the class comments.
  */
 CTCLObject
 CTreeVecActual::describeVector(
    CTCLInterpreter& interp, std::string name, CTreeParameterVector::pTreeVectorInfo pInfo
) {
    CTCLObject result;
    result.Bind(interp);

    Tcl::DictPut(interp, result, "name", name.c_str());

    CTCLObject low; low.Bind(interp);
    low = pInfo->s_low;
    Tcl::DictPut(interp, result, "low", low);

    CTCLObject high; high.Bind(interp);
    high = pInfo->s_high;
    Tcl::DictPut(interp, result, "high", high);

    Tcl::DictPut(interp, result, "units", pInfo->s_units.c_str());


    CTCLObject params = parameterList(interp, pInfo);
    Tcl::DictPut(interp, result, "parameters", params);


    return result;
}

/**
 * parameterList
 *     Create a Tcl List of the parameters a vector has created.
 * 
 * @param interp - interpreter running the command.
 * @param pInfo  - Pointer to the vector info block.
 * @return CTCLObject - list with the parameter names.
 */
CTCLObject
CTreeVecActual::parameterList(CTCLInterpreter& interp, CTreeParameterVector::pTreeVectorInfo pInfo) {
    CTCLObject result;
    result.Bind(interp);

    for (auto p : pInfo->s_createdParameters) {
        CTCLObject name; name.Bind(interp);
        name = p->getName();
        result += name;
    }
    return result;
}

/////////////////////////////////// MPI Wrapper.

/**
 * CTreeVecCommmand
 *    constructs the command we actually register.  in MPI land it will dispatch to 
 * the actual, for serial spectcl it will just all the actual's operator()
 * 
 * @param interp - the interpreter to register the command on:
 */
CTreeVecCommand::CTreeVecCommand(CTCLInterpreter& interp) :
    CMPITclCommandAll(interp, "treeparamvec", new CTreeVecActual(interp)) {}