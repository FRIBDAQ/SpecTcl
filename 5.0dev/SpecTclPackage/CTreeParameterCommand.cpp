/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include "CTreeParameterCommand.h"
#include "CSpecTclInterpreter.h"
#include "CParameterDictionary.h"
#include "CParameter.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <TCLException.h>
#include <iostream>


/** 
 * @file CTreeParameterCommand.cpp
 * @brief Implements the ::spectcl::treeparameter command ensemble.
 */

/**
 * static data
 */

static const  char* operationTexts[] = {
  "addFirst", "addReference", "removeReference", "removeLast"
};

/**
 * static member data
 */

CTreeParameterCommand* CTreeParameterCommand::m_pInstance(0);


/**
 * constructor
 *   Constructs a new instance of the tree parameter command
 */
CTreeParameterCommand::CTreeParameterCommand() :
  CTCLObjectProcessor(*(CSpecTclInterpreter::instance()), "::spectcl::treeparameter",
		      true), m_pObserver(0) {}

/**
 * destructor
 *    Gets rid of any observer and trace script objects.
 */
CTreeParameterCommand::~CTreeParameterCommand() {
  CParameterDictionary* pDict = CParameterDictionary::instance();
  if(pDict && m_pObserver) {
    pDict->removeObserver(m_pObserver);
  }
  delete m_pObserver;
  m_pObserver = 0;

  while (!m_traceScripts.empty()) {
    CTCLObject* pObject = m_traceScripts.front();
    delete pObject;
    m_traceScripts.pop_front();
  }
}


/**
 * instance
 *
 *  If necessary creates an instance of  CTreeParameterCommand
 *  returning the instance pointer.  This method enforces the
 *  singleton-ness of the object.
 *
 * @return CTreeParameterCommand*
 */
CTreeParameterCommand*
CTreeParameterCommand::instance()
{
  if (! m_pInstance) {
    m_pInstance = new CTreeParameterCommand;
  }
  return m_pInstance;
}

/**
 * operator()
 *
 *   This method is called when the ::spectcl::treeparameter command is recognized.
 *   This method really just dispatches based on the subcommand.
 *   *  It is an error for there not to be a subcommand.
 *   *  It is an error for the subcommand not to be one of those described
 *      in the class comment header.
 *
 * @param interp - The interpreter that is running the command.
 * @param objv   - Array of command words as object encapsulated Tcl_Obj*'s.
 *
 * @return int
 * @retval TCL_OK Successful completion.  Result varies depending on the
 *                subcommand.
 * @retval TCL_ERROR Failure. Result is a human readable error message.
 * 
*/
int
CTreeParameterCommand::operator()(CTCLInterpreter& interp, 
				  std::vector<CTCLObject>& objv)
{
  try {
    requireAtLeast(objv, 2, "::spectcl::treeparameter missing subcommand");
    bindAll(interp, objv);
    std::string subcommand = (std::string)(objv[1]);

    if (subcommand == "create") {
      create(interp, objv);
    } else if (subcommand == "list") {
      list(interp, objv);
    } else if (subcommand == "modify") {
      modify(interp, objv);
    } else if (subcommand == "setbins") {
      setbins(interp, objv);
    } else if (subcommand == "setlimits") {
      setlimits(interp, objv);
    } else if (subcommand == "setunits") {
      setunits(interp, objv);
    } else if (subcommand == "trace") {
      trace(interp, objv);
    } else {
      throw std::string("::spectcl::treeparameter bad subcommand");
    }
  }
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }
  return TCL_OK;
}
/**
 * create
 *   responsible for creating parameters.
 *   - there must be exactly 7 command line parameters.
 *   - The low and high must be valid doubles.
 *   - The low must be < high
 *   - The bins must be an unsigned integer > 0.
 *   - The parameter cannot already exist.
 *
 * @note all errors are signalled as std::string exceptions where the caller
 *       converts them into command status returns and the string into ther result.
 * @param interp - The interpreter that is running the command.
 * @param objv   - Array of command words as object encapsulated Tcl_Obj*'s.
 *
 * @note on success the result is set to the name of the tree parameter
 *       created.
 */
void
CTreeParameterCommand::create(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 7, 
		 "::spectcl::treeparameter create - incorrect number of parameters");
  std::string parameterName = objv[2];

  // Check for duplication which is not allowed here:
  
  CParameterDictionary* pDict = CParameterDictionary::instance();
  if (pDict->find(parameterName) != pDict->end()) {
    throw std::string("::spectcl::treeparameter create - parameter already exists");
  }
  ParameterInfo info;
  getProperties(objv, &info);

  new CParameter(parameterName.c_str(), info.s_low, info.s_high, info.s_bins, 
		 info.s_units.c_str());
  interp.setResult(parameterName);
  
  
}
/**
 * list
 *
 *  Produce a Tcl list that contains a list of the SpecTcl parameter that are
 *  known/defined.  This list includes those created by the create subcommand
 *  and those created by instantiating CParameter objects.
 *
 *  @note if the optional pattern parameter is missing it is assumed to be
 *        "*"
 *
 * @note all errors are signalled as std::string exceptions where the caller
 *       converts them into command status returns and the string into ther result.
 * @param interp - The interpreter that is running the command.
 * @param objv   - Array of command words as object encapsulated Tcl_Obj*'s.
 *
 * @note the interpreter result is a Tcl list, one element per tree parameter
 *       that matches the pattern.  Easch element of the list is itself a list
 *       that describes the parameter containing in order:
 *       * The parameter name
 *       * The parameter low limit
 *       * The parameter high limit
 *       * The suggested parameter binning
 *       * The parameter units - may be blank but will always be present.
 */
void
CTreeParameterCommand::list(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  // There must be at most 3 command words.

  requireAtMost(objv, 3, "::spectcl::treeparameter list - incorrect number of parameters");

  // Figure out what the matching pattern is:

  std::string match("*");
  if (objv.size() == 3) {
    match = (std::string)objv[2];
  }

  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator pItem = pDict->begin();
  CTCLObject result;
  result.Bind(interp);
  while (pItem != pDict->end()) {
    if (Tcl_StringMatch(pItem->first.c_str(), match.c_str())) {
      CTCLObject description;
      description.Bind(interp);
      
      CParameter* pParam = pItem->second->s_references.front();
      description += pParam->getName();
      description += pParam->getLow();
      description += pParam->getHigh();
      description += (int)pParam->getBins();
      description += pParam->getUnits();
      
      result += description;
    }
    pItem++;
  }
  interp.setResult(result);
  
}
/**
 * modify
 *
 *   Modifies all aspects of a parameter, low, high, bins, units.
 *
 * @note all errors are signalled as std::string exceptions where the caller
 *       converts them into command status returns and the string into ther result.
 * @param interp - The interpreter that is running the command.
 * @param objv   - Array of command words as object encapsulated Tcl_Obj*'s.
 *  
 * @note the Tcl result from this method on success is just the name of the
 *       parameter modified.  On error, an human readable error message is used.
 */
void
CTreeParameterCommand::modify(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 7, "::spectcl::treeparameter modify - Incorrect number of parameters");

  CParameterDictionary::DictionaryIterator pItem = findParameter(objv[2], "modify");

  ParameterInfo info;
  getProperties(objv, &info);
  CParameter* pParam = pItem->second->s_references.front();
  pParam->changeLow(info.s_low);
  pParam->changeHigh(info.s_high);
  pParam->changeBins(info.s_bins);
  pParam->changeUnits(info.s_units.c_str());

  interp.setResult(pItem->first);
}

/**
 * setbins
 *    Change the suggested binning for a parameter.
 *
 * @note all errors are signalled as std::string exceptions where the caller
 *       converts them into command status returns and the string into ther result.
 * @param interp - The interpreter that is running the command.
 * @param objv   - Array of command words as object encapsulated Tcl_Obj*'s.
 *
 * @note on success the name of the parameter is set as the command value.
 */
void
CTreeParameterCommand::setbins(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 4, "::spectcl::treeparameter setbins - Incorrect parameter count.");
  CParameterDictionary::DictionaryIterator pItem = findParameter(objv[2], "setbins");
  int newBins = objv[3];
  if (newBins <= 0) {
    throw std::string("::spectcl::treeparameter setbins - bin count must be > 0");
  }
  pItem->second->s_references.front()->changeBins(newBins);
  interp.setResult(pItem->first);
}

/**
 * setlimits
 *   Change the limits on a parameter.
 *   - Parameter must exist.
 *   - Limits must be double.
 *   - Low must be strictly less than high.
 *
 * @note all errors are signalled as std::string exceptions where the caller
 *       converts them into command status returns and the string into ther result.
 * @param interp - The interpreter that is running the command.
 * @param objv   - Array of command words as object encapsulated Tcl_Obj*'s.
 *
 * @note on success the name of the parameter is set as the command value.
 */
void
CTreeParameterCommand::setlimits(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 5, "::spectcl::treeparameter setlimits - incorrect number of parameters");
  CParameterDictionary::DictionaryIterator pItem = findParameter(objv[2], "setlimits");

  double low = objv[3];
  double high = objv[4];
  if (low >= high) {
    throw std::string("::spectcl::treeparameter - low must be < high");
  }
  pItem->second->s_references.front()->changeLow(low);
  pItem->second->s_references.front()->changeHigh(high);

  interp.setResult(pItem->first);
}
/**
 * setunits
 *
 *   Changes the units of measure for a tree parameter.
 *   -  There must be 4 parameters.
 *   -  The parameter named must exist.s
 *
 * @note all errors are signalled as std::string exceptions where the caller
 *       converts them into command status returns and the string into ther result.
 * @param interp - The interpreter that is running the command.
 * @param objv   - Array of command words as object encapsulated Tcl_Obj*'s.
 *
 * @note on success the name of the parameter is set as the command value.
 */
void
CTreeParameterCommand::setunits(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv,4, 
		 "::spectcl::treeparameter setunits - incorrect number of parameters");
  CParameterDictionary::DictionaryIterator pItem = findParameter(objv[2], "setunits");
  std::string units = objv[3];

  pItem->second->s_references.front()->changeUnits(units.c_str());
  
  interp.setResult(pItem->first);
}
/**
 * trace
 *   Dispatches the trace command.  Trace has the following sub/sub commands:
 *   - add - adds a new trace to the observer.
 *   - remove - Removes a trace from the observe.
 *   - show   - Lists the traces that are already there.
 *
 * @note all errors are signalled as std::string exceptions where the caller
 *       converts them into command status returns and the string into ther result.
 * @param interp - The interpreter that is running the command.
 * @param objv   - Array of command words as object encapsulated Tcl_Obj*'s.
 * 
 */
void
CTreeParameterCommand::trace(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireAtLeast(objv, 3,
		 "::spectcl::treeparameter trace - needs an add, remove or show subcommand.");
  std::string operation = objv[2];
  if (operation == "add") {
    traceAdd(interp, objv);
  } else if (operation == "remove") {
    traceRemove(interp, objv);
  } else if (operation == "show") {
    traceShow(interp, objv);
  } else {
    throw std::string("::spectcl::treeparameter trace - invalid subcommand.");
  }
}
/**
 * traceAdd
 *    Handle the trace add subcommand.  A trace is added that gets called
 *    in the same manner as a dictionary observer will.   This is done by
 *    creating(if necessary) an observer that contains an ordered list of
 *    of script objects.  These scripts are called after appending:
 *    - The name of the affected parameter.
 *    - The textualized operation:  "addFirst", "addReference", "removeReference",
 *      "removeLast"
 *
 * @note all errors are signalled as std::string exceptions where the caller
 *       converts them into command status returns and the string into ther result.
 * @param interp - The interpreter that is running the command.
 * @param objv   - Array of command words as object encapsulated Tcl_Obj*'s.
 *
 * No result is produced by a successful completion.
 */
void
CTreeParameterCommand::traceAdd(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 4, 
		 "::spectcl::treeparameter trace add - incorrect parameter count.");
  
  /* 
    If we have not already registered our observer with the parameter
    dictionary:
    * Create it.
    * Register it.
    * Remember it so that the destructor can do all the right things.
    */
  if (!m_pObserver) {
    m_pObserver = new CScriptObserver;
    CParameterDictionary* pDict = CParameterDictionary::instance();
    pDict->addObserver(m_pObserver);
  }
  // Regardless add a copy of  script to the back end of the traceScripts list:

  CTCLObject* pScriptCopy = new CTCLObject(objv[3]);
  pScriptCopy->Bind(interp);
  m_traceScripts.push_back(pScriptCopy);
} 
/**
 * traceRemove
 *
 *   Removes the specified trace from the list of traces.
 *
 * @note all errors are signalled as std::string exceptions where the caller
 *       converts them into command status returns and the string into ther result.
 * @param interp - The interpreter that is running the command.
 * @param objv   - Array of command words as object encapsulated Tcl_Obj*'s.
 *
 * No result is produced by a successful completion.
 */
void
CTreeParameterCommand::traceRemove(CTCLInterpreter& interp, 
				   std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 4, 
		 "::spectcl::treeparameter trace remove - incorrect parameter count.");
  std::string script = objv[3];
  bool found = false;
  for(std::list<CTCLObject*>::iterator p = m_traceScripts.begin(); 
      (!found) & (p != m_traceScripts.end());  p++) {

    CTCLObject* pScript = *p;
    if (script == (std::string)(*pScript)) {
      m_traceScripts.erase(p);
      found = true;
    }
  }
  if (!found) {
    throw std::string("::spectcl::treeparameter trace remove - no such trace procedure");
  }
}
/**
 * traceShow
 *   Produce a Tcl list consisting of the trace scripts currently defined.
 *
* @note all errors are signalled as std::string exceptions where the caller
 *       converts them into command status returns and the string into ther result.
 * @param interp - The interpreter that is running the command.
 * @param objv   - Array of command words as object encapsulated Tcl_Obj*'s.
 */
void
CTreeParameterCommand::traceShow(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 3,
		 "::spectcl::treeparameter trace show - incorrect number of parameters.");
  CTCLObject result;
  result.Bind(interp);
  for(std::list<CTCLObject*>::iterator p = m_traceScripts.begin(); 
      p != m_traceScripts.end(); p++) {
    result += **p;
  }
  interp.setResult(result);
}

/*----------------------------------------------------------------------
 * Private utilities
 */

/**
 * getProperties
 *
 *  Gets the properties of a parameter from the command line objects.
 *  The properties are assumed to be  in order
 *  low, high, bins, units.
 *
 * @param objv - Vector of command line objects.
 * @param pInfo - Points to a ParameterInfo struct that will be filled in with the
 *                data.
 * @Param startingAt - Which command word contains low.
 *
 * @note errors are signalled via exceptions.
 * @note all validations of the parameters are also performed.
 */
void
CTreeParameterCommand::getProperties(std::vector<CTCLObject>& objv, 
				     CTreeParameterCommand::pParameterInfo pInfo, 
				     unsigned startingAt)
{
  try {
    pInfo->s_low  = (double)objv[startingAt];
    pInfo->s_high = (double)objv[startingAt+1];
    if (pInfo->s_low >= pInfo->s_high) {
      throw std::string("::spectcl::treeparameter - low must be < high");
    }
    int bins = (int)objv[startingAt+2];
    if (bins <= 0) {
      throw std::string("::spectcl::treeparameter - Bins parameter must be greater than zero");
    }
    pInfo->s_bins = bins;


    // Anything goes for units:

    pInfo->s_units = (std::string)objv[startingAt + 3];
 
  }
  catch (CTCLException& e) {
    throw std::string(e.ReasonText());
  }
}
/**
 * findParameter
 *
 *  Given a CTCLObject, locates and returns the parameter iterator to it
 *  in the parameter dictionary or throws a string exception if the parameter
 *  is not in the dictionary.
 *
 * @param obj - refers to the command line word containing the 
 *              name to find.
 * @param pSubcommand - Subcommand being executed.  This is wrapped into the
 *                      exception string.
 *
 * @return CParameterDictionary::DictionaryIterator that 'points' to the
 *                       dictionary key/value pair for the parameter.
 */
CParameterDictionary::DictionaryIterator
CTreeParameterCommand::findParameter(CTCLObject& obj, const char* pSubcommand)
{
  std::string name           = (std::string)(obj);
  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator pItem = pDict->find(name);

  if (pItem == pDict->end()) {
    std::string msg = "::spectcl::treeparameter ";
    msg += pSubcommand;
    msg += " - parameter does not exist.";
    throw msg;
  }
  return pItem;
}
/**
 * runTraces
 *
 *   This is called back from the script observer
 *   - marshalls the operation info into textual format.
 *   - for each script in the list, appends the parameters produced above.
 *     and calls the script.
 * @todo need to decide what to do if a script fails.
 *
 *  @param pInfo - Info about what is being traced and why.
 */
void
CTreeParameterCommand::runTraces(const CParameterDictionary::pOperationInfo pInfo)
{
  const char* opString = operationTexts[pInfo->s_op];
  CTCLInterpreter* pInterp = getInterpreter();

  // Make the fixed part of the command.

  CTCLObject name;
  name.Bind(pInterp);
  name = pInfo->s_Name;

  CTCLObject op;
  op.Bind(pInterp);
  op = opString;

  Tcl_Obj* objv[3];
  objv[1] = name.getObject();
  objv[2] = op.getObject();

  // For each trace script put it's object ptr in objv[0] and execute
  // the vector of objects.

  for (std::list<CTCLObject*>::iterator p = m_traceScripts.begin();
       p != m_traceScripts.end(); p++) {
    CTCLObject* pScript = *p;
    objv[0] = pScript->getObject();

    int stat = Tcl_EvalObjv(pInterp->getInterpreter(), 3, objv, TCL_EVAL_GLOBAL);

    if (stat == TCL_ERROR) {
      std::cerr << "call back failed: " 
		<< Tcl_GetStringResult(pInterp->getInterpreter()) << std::endl;

    }
  }
}

/*----------------------------------------------------------------
 * Inner class implementations

 /**
 * CScriptObserver::operator() - relay to runTraces.
 *
 * @param pDict - pointer to the parameter dictionary object.
 */
void
CTreeParameterCommand::CScriptObserver::operator()(CObservable<CParameterDictionary>* pDict)
{
  CTreeParameterCommand* pCommand = CTreeParameterCommand::instance();
  CParameterDictionary*  pD      = reinterpret_cast<CParameterDictionary*>(pDict);
  pCommand->runTraces(pD->getOperation());
}
