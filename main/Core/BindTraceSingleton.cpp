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

/** @file:  BindTraceSingleton.cpp
 *  @brief: Implement a singleton to hold bind/unbind traces.
 */

#include "BindTraceSingleton.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>

#include <stdexcept>

// The singleton instance pointer:

BindTraceSingleton* BindTraceSingleton::m_pInstance(0);

/**
 * BindTraceSingleton - constructor (private)
 *    For now there's nothing to do, but we need to define it to make it private.
 */
BindTraceSingleton::BindTraceSingleton()
{}

/**
 *  getInstance [static]
 *
 *  @return BindTraceSingleton& - reference to the singleton instance.
 *  @note if necessary the singleton instance is created.
 */
BindTraceSingleton&
BindTraceSingleton::getInstance()
{
    if (!m_pInstance) {
        m_pInstance = new BindTraceSingleton;
    }
    return *m_pInstance;
}

/**
 * addSbindTrace
 *    Adds a new sbind trace.  Multiple traces are supported in an ordered
 *    manner.
 * @param interp - interpreter into which the script object will be bound.
 * @param script - stem of the script called when an sbind occured.
 *
 */
void
BindTraceSingleton::addSbindTrace(CTCLInterpreter& interp, CTCLObject& script)
{
    add(m_sbindTraces, interp, script);
}
/**
 * removeSbindTrace
 *    Removes the matching stem script from the sbind traces.
 *  @param  script - bound script. The stem script who's string representation
 *          exactly matches us is removed.
 *  @throw std::invalid_argment if there's no match.
 */
void
BindTraceSingleton::removeSbindTrace(CTCLObject& script)
{
    remove(m_sbindTraces, script);
}
/**
 * andUnbindTrace
 *    Add a new trace to the unbind callback list.
 * @param interp -interpreter used to bind the script copy.
 * @param script - Stem of trace script
 */
void
BindTraceSingleton::addUnbindTrace(CTCLInterpreter& interp, CTCLObject& script)
{
    add(m_unbindTraces, interp, script);
}
/**
 * removeUnbindScript
 *  Remove a script from unbind callback list.
 *  
 *  @param  script - bound script. The stem script who's string representation
 *          exactly matches us is removed.
 *  @throw std::invalid_argment if there's no match.
 */
void
BindTraceSingleton::removeUnbindTrace(CTCLObject& script)
{
    remove(m_unbindTraces, script);
}

/**
 * invokeSbind
 *    Invoke the sbind traces.
 * @param interp - interpreteter that runs the traces.
 * @param name   - Name of spectrum being sbound.
 * @param id     - Xamine id into which the binding happened.
 * @throw CTCLException - if one of the traces errors.
 * @note if a trace script errors trace processing ends with that
 *       script.
 */
void
BindTraceSingleton::invokeSbind(
   CTCLInterpreter& interp, CTCLObject& name, CTCLObject& id
)
{
   
   invoke(m_sbindTraces, interp, name, id);    
}
/**
 * invokeUnbind
 *    Invoke the unbind traces.
 * @param interp - interpreteter that runs the traces.
 * @param name   - Name of spectrum being unbound.
 * @param id     - Xamine id from which the spectrum is being unbound.
 * 
 */
void
BindTraceSingleton::invokeUnbind(
    CTCLInterpreter& interp, CTCLObject& name, CTCLObject& id
)
{
    invoke(m_unbindTraces, interp, name, id);   
}

///////////////////////////////////////////////////////////////////////////////
// private utilties.

/**
 * add
 *    Add a callback to a list.
 * @param cbs  - callback list.
 * @param interp - intepreter used to bind the copy of the script.
 * @param script - stem of script to add.
 */
void
BindTraceSingleton::add(
    std::list<CTCLObject*>& cbs, CTCLInterpreter& interp, CTCLObject& script
)
{
    CTCLObject* pScriptCopy = new CTCLObject(script);
    pScriptCopy->Bind(interp);
    cbs.push_back(pScriptCopy);
}
/**
 * remove
 *    Remove a callback from a list.
 *  @param cbs - callback list.
 *  @param script - script stem. Must match the string representation of the
 *                  stored callback script.
 *  @throw std::invalid_argument - there's no matching script.
 */
void
BindTraceSingleton::remove(std::list<CTCLObject*>& cbs, CTCLObject& script)
{
    bool found(false);
    std::string strScript = script;
    for (auto p = cbs.begin(); p != cbs.end(); ++p) {
        std::string stem = *(*p);
        if (stem == strScript) {
            found = true;
            delete *p;
            cbs.erase(p);
        }
    }
    if (!found) {
        std::string msg("Could not find/remove callback: ");
        msg += strScript;
        throw std::invalid_argument(msg);
    }
}
/**
 * invoke - invoke the callbacks from a list.
 *
 * @param cbs - callback list.
 * @param interp - interpreter runing the command.
 * @Param name  - spectrum name.
 * @param id    - Xamine id.
 */
void
BindTraceSingleton::invoke(
    std::list<CTCLObject*>& callbacks,
    CTCLInterpreter& interp, CTCLObject& spectrumName, CTCLObject& xamineId
)
{
    for (auto& stem : callbacks) {
        CTCLObject script;
        script.Bind(interp);
        script += *stem;
        script += spectrumName;
        script += xamineId;
        
        script();
    }
    
}