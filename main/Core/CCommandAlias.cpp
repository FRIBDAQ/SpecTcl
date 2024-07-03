/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2024

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Aaron Chester
             Simon Giraud
             Giordano Cerizza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include "CCommandAlias.h"

///////////////////////////////// Implement CCommandAlias ///////////////////////////////////


/**
 * constructor 
 *    @param interp - pointer to the interpreter.
 *    @param alias - Name of the aliasing command.
 *    @param pActual - The processor for the original command note this is assumed to be dynamically
 * allocated (new) and our destruction will delete it.
 */
CCommandAlias::CCommandAlias(CTCLInterpreter* pInterp, const char* alias, CTCLObjectProcessor* pActual) :
    CTCLObjectProcessor(*pInterp, alias, TCLPLUS::kfTRUE),
    m_pActual(pActual) {}

/**
 *  destructor
 *    Just delete the wrapped command processor.
 */
CCommandAlias::~CCommandAlias() {
    delete m_pActual;
}

/**
 *  operator() - relay to the actual processor.
 *    @param interp - references the interpreter.
 *    @param objv - the command words.
 */
int
CCommandAlias::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    return (*m_pActual)(interp, objv);
}

/////////////////////////// Implement CPackagedCommandAlias

/**
 *  constructor
 *    @param interp - Interpreter to register on.
 *    @param alias - Additional command under which to register.
 *    @param pActual - pointer to the command processor we're wrapping.  This assumed to have ben
 *    @param package - the package we are in.
 *     dynamically created via new and our destructor will delete it.
 */
CPackagedCommandAlias::CPackagedCommandAlias(
    CTCLInterpreter& interp, const char* alias, 
    CTCLPackagedCommand* pActual, CTCLCommandPackage& package
) :
    CTCLPackagedCommand(alias, &interp, package),
    m_pActual(pActual) {}

/**
 *  destructor
 */
CPackagedCommandAlias::~CPackagedCommandAlias() {
    delete m_pActual;
}

/**
 *  operator() command relay.
 *
 *   @param interp - the interpreter.
 *   @param result - the result.
 *   @param arg, argv - command line arguments.
 */
int
CPackagedCommandAlias::operator() (
    CTCLInterpreter& interp, CTCLResult& result, int argc, char* argv[]
) {
    return (*m_pActual)(interp, result, argc, argv);
}

////////////////////////////////// CPackagedObjectProcessorAlias ///////////////////////////////////


/**
 *  constructor
 *      @param interp - interpreter
 *      @param alias - Command alias.
 *      @param pActual - Actual command processor.  Will be deleted by our destruction.
 * 
 * @note pActual must have already been attached to the owning package or be so externally.  The
 *    interface for attachment does not really allow us to get control and relay that.
 */
CPackagedObjectProcessorAlias::CPackagedObjectProcessorAlias(
    CTCLInterpreter& interp, std::string alias, CTCLPackagedObjectProcessor* pActual
) :
    CTCLPackagedObjectProcessor(interp, alias),
    m_pActual(pActual) {}
/** 
 * destructor
 * 
 */
CPackagedObjectProcessorAlias::~CPackagedObjectProcessorAlias() {
    delete m_pActual;
}


/**
 *  operator()  - relay to the actual command.
 * 
 *   @param interp - interpreter.
 *   @param objv  - the command words.
 */
int
CPackagedObjectProcessorAlias::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    return (*m_pActual)(interp, objv);
}
