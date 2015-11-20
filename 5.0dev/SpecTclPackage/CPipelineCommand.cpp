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
#include "CPipelineCommand.h"
#include "CSpecTclInterpreter.h"
#include <TCLObject.h>
#include "CDecoder.h"
#include "CEventProcessor.h"
#include "CAnalysisPipeline.h"

#include <assert.h>

/**
 * @file CPipelineCommand.cpp
 * @brief Implement the spectcl::pipeline command ensemble.
 */


CPipelineCommand* CPipelineCommand::m_pInstance(0);

/**
 * constructor 
 *
 *  Locate the interpreter and register us as ::spectcl::pipeline command.
 *  It is required that the ::spectcl:: namespace already exists.
 */
CPipelineCommand::CPipelineCommand() :
  CTCLObjectProcessor(*CSpecTclInterpreter::instance(), "::spectcl::pipeline") 
{}
/**
 * destructor
 *   A no-op
 */
CPipelineCommand::~CPipelineCommand()
{}

/**
 * instance
 * 
 *  Return the singleton instance of the object.  This also constructs the object
 *  if needed.  This normally should be called in the package initializer as well 
 *  so that package require defines the command.
 *
 * @return CPipelineCommand*
 */
CPipelineCommand*
CPipelineCommand::instance()
{
  if (!m_pInstance) {
    m_pInstance = new CPipelineCommand;
  }
  return m_pInstance;

}

/*--------------------------------------------------------------------------
** Command handling and dispatch:
*/

/**
 * operator()
 *
 *  Control is transferred here to execute the command.  
 *  *  Ensure there's a subcommand.
 *  *  Dispatch to the appropriate processor depending on the subcommand.
 *  *  Exceptions are caught and converted to interpreter results and
 *     TCL_ERROR returns.
 *
 * @param interp - The interpreter executing this command.
 * @param objv   - vector of encapsulated Tcl_Obj's that make up the command words.
 *
 * @return int - TCL_OK on success, TCL_ERROR on failure.
 */
int
CPipelineCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  // Everything is in a try/catch block as that's how errors will be reported
  // the catch will convert them into result/TCL_ERROR.

  try {
    // Need to have at least a subcommand:
    
    
    requireAtLeast(objv, 2, "Insufficient parameters, need at least a subcommand");
    bindAll(interp, objv);
    
    std::string subCommand = objv[1];
    if (subCommand == "setdecoder") {
      setDecoder(interp, objv);
    } else if (subCommand == "showdecoder") {
      showDecoder(interp, objv);
    } else if (subCommand == "add") {
      addElement(interp, objv);
    } else if (subCommand == "list") {
      listElements(interp, objv);
    } else if (subCommand == "remove") {
      removeElement(interp, objv);
    } else {
      throw std::string("spectcl::pipeline invalid subcommand");
    }
  }
  catch(std::string errorMessage) {
    interp.setResult(errorMessage);
    return TCL_ERROR;
  }
  catch(std::exception& e) {
    interp.setResult(e.what());
    return TCL_ERROR;
  }

  return TCL_OK;
}

/**
 * addElement
 *
 *  Called to execute the add subcommand which adds an element to
 *  the event processing pipeline.
 *  There must be at least 3 and at most 7 command words.
 *  see the class header file for more information about 
 *  the syntaxes recognized by this command.
 *  *  Check the correct parameter count.
 *  *  Find the event processor and throw an error if it does not exist.
 *
 * @param interp - The interpreter executing this command.
 * @param objv   - vector of encapsulated Tcl_Obj's that make up the command words.
 *
 */
void
CPipelineCommand::addElement(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv)
{
  // Check the parameter counts:

  requireAtLeast(objv, 3, "Insufficient command parameters");
  requireAtMost(objv, 7, "Too many command parameters");

  /*
    Pull out the event processor name, get the event processor and be pissed
    off if it's not been registered.
  */
 
  std::string evpName = objv[2];
  pEventProcessor pEp = findEventProcessor(evpName);
  if (pEp == m_RegisteredEventProcessors.end()) {
    std::string msg = "no such event processor : '";
    msg += evpName;
    msg += "'";
    throw msg;
  }

  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();

  // Setup for processKeywords:

  Keywords keys;
  keys["before"] = KeywordData();
  keys["to"]     = KeywordData("event");

  processKeywords(objv, 3, keys);

  std::string pipeName = keys["to"].value;

  if (pipeName == "event") {
    if (keys["before"].present) {
      pPipe->addElement(pEp->second, keys["before"].value.c_str());
    } else {
      pPipe->addElement(pEp->second);
    }
  } else if (pipeName == "analysis") {
    if (keys["before"].present) {
      pPipe->addAnalysisElement(pEp->second, keys["before"].value.c_str());
    } else {
      pPipe->addAnalysisElement(pEp->second);
    }
  } else {
    std::string msg = "no such pipeline: '";
    msg += pipeName;
    msg += "' must be 'event' or 'analysis'";
    throw msg;
  }

  interp.setResult(evpName);
    

}
/**
 * listElements
 *
 *   Lists the elements in the analysis pipeline in the order in which they occur
 *   in the pipeline.
 *
 * @param interp - The interpreter executing this command.
 * @param objv   - vector of encapsulated Tcl_Obj's that make up the command words.
 *
 *  The command result is set with the listing.
 */
void
CPipelineCommand::listElements(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv)
{
  CTCLObject resultList;
  resultList.Bind(interp);

  Keywords keys;
  keys["matching"] = KeywordData("*");
  keys["in"]    = KeywordData("event");
  processKeywords(objv, 2, keys);

  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
  std::string pipeline = keys["in"].value;
  CAnalysisPipeline::PipelineIterator p;
  CAnalysisPipeline::PipelineIterator end;
  if (pipeline == "event") {
    p = pPipe->begin();		//  Use the event pipeline iterators.
    end = pPipe->end();
  } else if (pipeline == "analysis") {
    p = pPipe->analysisBegin();	// Usethe analysis pipeline iterators.
    end = pPipe->analysisEnd();    
  } else {
    // Bad pipeline name.
    std::string msg("invalid pipeline name '");
    msg += pipeline;
    msg += "' must be 'event' or 'analysis'";
    throw msg;
  }
  // Iterate on whichever pipeline is appropriate:
  while(p != end) {
    std::string name = (*p)->getName();
    if(Tcl_StringMatch(name.c_str(), keys["matching"].value.c_str())) {
      resultList += (*p)->getName();
    }
    p++;
  }
  interp.setResult(resultList);
}
/**
 * removeElement
 *   Called to remov an element from a pipeline.
 *   See the header for the full syntax and semantics
 *   (::spectcl::pipeline remove $name ?in $which-pipeline?).
 *
 * @param interp - The interpreter executing this command.
 * @param objv   - vector of encapsulated Tcl_Obj's that make up the command words.
 *
 */
void
CPipelineCommand::removeElement(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv)
{
  // There must be at least an element name:

  requireAtLeast(objv, 3);
  std::string elementName = objv[2];

  Keywords keys;
  keys["in"] = KeywordData("event");
  processKeywords(objv, 3, keys);

  // ensure the element exists...else that's reason to fail.

  CAnalysisPipeline*                  pPipe    = CAnalysisPipeline::instance();

  CAnalysisPipeline::PipelineIterator pElement;
  CAnalysisPipeline::PipelineIterator end;
  std::string pipeName = keys["in"].value;
  if (pipeName == "event") {
    pElement = pPipe->find(elementName.c_str());
    end = pPipe->end();
  } else if (pipeName == "analysis") {
    pElement = pPipe->analysisFind(elementName.c_str());
    end = pPipe->analysisEnd();
  } else {
    std::string msg("invalid pipeline: '");
    msg += pipeName;
    msg += "' must be 'analysis' or 'event'";
    throw msg;
  }

  if (pElement == end) {
      std::string msg("no such element '");
      msg += elementName;
      msg += "'";
      throw msg;
  } else {
    pipeName == "event" ? 
      pPipe->erase(pElement) : pPipe->eraseAnalyzer(pElement);
  }
}

/**
 * setDecoder
 *
 *   Called to execute the setdecoder subcommand:
 *  *  Ensure there is a decoder name (and only a decoder name).
 *  *  Ensure the name corresponds to a decoder.
 *  *  Get the AnalysisPipeline.
 *  *  Set its decoder appropriately.
 *
 * @param interp - The interpreter executing this command.
 * @param objv   - vector of encapsulated Tcl_Obj's that make up the command words.
 *
 */
void
CPipelineCommand::setDecoder(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 3, "Incorrect number of command parameters");

  std::string decoderName = objv[2];
  pDecoder pD = findDecoder(decoderName);
  if (pD == m_RegisteredDecoders.end()) {
    std::string msg = "no such decoder : '";
    msg += decoderName;
    msg += "'";
    throw msg;
  }

  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
  pPipe->setDecoder(pD->second, decoderName);

}
/**
 * showDecoder
 *
 *  Called to show the type of decoder currently held by the
 *  analysis pipeline
 *  * Must be no other command parameter.
 *  * Get the pipeline instance.
 *  * Use getDecoderType to get the current type string.
 *
 * @param interp - The interpreter executing this command.
 * @param objv   - vector of encapsulated Tcl_Obj's that make up the command words.
 *
 */
void
CPipelineCommand::showDecoder(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv)
{
  CAnalysisPipeline* pPipe  = CAnalysisPipeline::instance();
  interp.setResult(pPipe->getDecoderType());
}

/*------------------------------------------------------------------------------
** Decoder 'factory' management.
*/

/**
 * defineDecoder 
 *
 *  Add a decoder to the type of decoders that are known to the pipeline command.
 *  These can be used with the analysis pipeline via the spectcl::pipeline setdecoder
 *  command.
 *
 * @param decoderType - The decoder type, this is what you use in the decodername
 *                      parameter of the spectcl::pipeline setdecoder command.
 * @param pDecoder    - Pointer to the decoder that will be installed when the
 *                      decoder type above is specified.
 */
void
CPipelineCommand::defineDecoder(std::string decoderType, CDecoder* pDecoder)
{
  if (findDecoder(decoderType) == m_RegisteredDecoders.end()) {
    m_RegisteredDecoders[decoderType] = pDecoder;
  } else {
    std::string msg = "CPipelineCommand::defineDecoder decoder is already defined: ";
    msg += decoderType;
    throw pipelinecommand_exception(msg.c_str());
  }
}

/**
 * removeDecoder
 *    Removes a decoder definition.  This makes it impossible to do a 'setdecoder'
 *    subcommand that specifies the given decoder.  It is an error to
 *    *  Try to remove a decoder that does not exist.
 *    *  Try to remove the decoder currently set in the pipeline.
 *
 * @param decoderType - The type of decoder being removed.
 *
 * @return CDecoder* Pointer to the removed decoder.  It is the caller's
 *                   responsibility to do storage management.
 *
 *  @throw pipelinecommand_exception - if there is an error.
 */
CDecoder*
CPipelineCommand::removeDecoder(std::string decoderType) 
{
  // If the decoder does not exist, throw


  CDecoder* pDecoderInstance(0);
  pDecoder p = findDecoder(decoderType);
  if (p == m_RegisteredDecoders.end()) {
    std::string msg = "CPipelineCommand::removeDecoder no such decoder '";
    msg += decoderType;
    msg += "'";
    throw pipelinecommand_exception(msg.c_str());
  } else {
    CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
    if (pPipe->getDecoderType() == decoderType) {
      std::string msg("CPipelineCommand::removeDecoder '");
      msg += decoderType;
      msg += "' is the current decoder";
      throw pipelinecommand_exception(msg.c_str());
    } else {
      pDecoderInstance = p->second;
      m_RegisteredDecoders.erase(p);
    }
  }

  return pDecoderInstance;
}

/*------------------------------------------------------------------------
** event pipeline factory management.
*/

/**
 * defineElement
 *  
 *  Add an event processor to the set that can be added via the 
 *  spectcl::pipeline add command.
 *
 * @param pProcessor - Event processor to add, The event processor is added
 *                     under it's name (what comes back from getName()).
 */
void
CPipelineCommand::defineElement(CEventProcessor* pProcessor)
{
  std::string name = pProcessor->getName();
  if(findEventProcessor(name) == m_RegisteredEventProcessors.end()) {
    m_RegisteredEventProcessors[name] = pProcessor;
  } else {
    std::string msg = "CPipelineCommand::defineElement - duplicate event processor: ";
    msg += name;
    throw pipelinecommand_exception(name.c_str());
  }
}
/**
 * removeElementDefinition
 *
 *  Removes an element definition from the registration set.
 *  Storage management for the element is up to the caller.
 *
 *  * It is an error to specify an element that is not registered.
 *  * It is an error to specify an element that is currently in the
 *    pipeline
 *
 * @param elementType - the name under which the element was registered.
 *
 *  @return CDecoder* - Pointer to the removed decoder.
 */
CEventProcessor*
CPipelineCommand::removeElementDefinition(std::string elementType)
{
  CEventProcessor* pRemoved(0);
  pEventProcessor p = findEventProcessor(elementType);
  if (p == m_RegisteredEventProcessors.end()) {
    std::string msg("CPipelineCommand::removeElementDefinition no such element '");
    msg += elementType;
    msg += "'";
    throw pipelinecommand_exception(msg.c_str());
  } else {
    CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
    if ((pPipe->find(elementType.c_str()) == pPipe->end())&&
	(pPipe->analysisFind(elementType.c_str()) == pPipe->analysisEnd())) {
      pRemoved = p->second;
      m_RegisteredEventProcessors.erase(p);
    } else {
      std::string msg("CPipelineCommand::removeElementDefinition '");
      msg += elementType;
      msg += "' is still in the pipeline and cannot be removed";
      throw pipelinecommand_exception(msg.c_str());
    }
  }
		    
  return pRemoved;
}
/*----------------------------------------------------------------------------
** Private utilities for testing etc.
*/

/**
 * decodersSize
 *
 * @return size_t - number of decoders registered.
 */
size_t
CPipelineCommand::decodersSize() const
{
  return m_RegisteredDecoders.size();
}

/**
 * findDecoder
 *   Returns an iterator to the specified decode, or m_RegisteredDecoders.end()
 *   if the decoder does not exist.
 *
 * @param type - Type of decoder we're finding.
 *
 * @return CPipelineCommand::pDecoder
 */
CPipelineCommand::pDecoder
CPipelineCommand::findDecoder(std::string type)
{
  return m_RegisteredDecoders.find(type);
}
/**
 * eventProcessorsSize
 *
 *   Returns the number of event processors that have been registered.
 *
 * @return size_t
 */
size_t
CPipelineCommand::eventProcessorsSize()
{
  return m_RegisteredEventProcessors.size();
}

/**
 * findEventProcessor
 *
 *  Get an iterator to the named event processor.
 *
 * @param name - Name to look for.
 *
 * @return CPipelineCommand::pEventProcessor
 * @retval m_RegisteredEventProcessors.end() - if not found.
 */
CPipelineCommand::pEventProcessor
CPipelineCommand::findEventProcessor(std::string name)
{
  return m_RegisteredEventProcessors.find(name);
}

/**
 * processKeywords
 *
 *   Processes a bunch of keyword/value pairs.
 *
 * @param obvj  - The Tcl_Objs that make up the command encapsulated in CTCLObjects. 
 *                These are assumed to have been bound to the interpreter.
 * @param startingAt - The index of the first objv[] itemt to be processed.
 * @param keywordMap - The map indexed by keyword whose values are the structs that describe
 *                     the presence of and the values of the keywords (caller can init these with
 *                     default values too).
 */
void
CPipelineCommand::processKeywords(
    std::vector<CTCLObject>& objv, unsigned startingAt, 
    CPipelineCommand::Keywords& keywordMap)
{
  std::string keyword;
  for(int i = startingAt; i < objv.size(); i += 2) {
    requireAtLeast(objv, i+2);	// need keyword and value.
    keyword = std::string(objv[i]);
    std::string value   = objv[i+1];

    if (keywordMap.find(keyword) != keywordMap.end()) {
      // Keyword is defined.

      KeywordData& item = keywordMap[keyword];
      item.present = true;
      item.value   = value;

    } else {
      // keyword is undefined.
      
      std::string msg("invalid keyword: '");
      msg += keyword;
      msg += "' must be ";
      msg += keylist(keywordMap);
      throw msg;
    }

  }
}
/**
 * keylist
 *   Joins a the list of keywords for a keyword map alphabetically producing a string
 *   of the form 'key1', 'key2' ... or 'keyn'
 *   Helper for the exception string generation in processKeywords above.
 *   By the nature of map iteration, the keywords will be in alpha order.
 *
 *  @param keywordMap - The keyword map to process.
 *
 *  @return std::string - as described above.
 */
std::string
CPipelineCommand::keylist(Keywords& keywordMap)
{
  std::string result;
  size_t      nKeys = keywordMap.size();
  size_t      i = 1;
  for(KeywordIterator p = keywordMap.begin(); p != keywordMap.end(); p++) {
    std::string keyword = p->first;

    // Append the quoted keyword:

    result += "'";
    result += keyword;
    result += "'";

    // What is appended as a separator depends on how many keywords are left:

    size_t remaining = nKeys - i;
    if (remaining == 1) { 
      result += " or ";		// Next to last gets an or.
    } else if (remaining > 1) {
      result += ", ";		// prior to next to last gets a ", "
    }				// Last one gets no separator.
    i++;
  }
  return result;
}

/*------------------------------------------------------------------------------------------
** testing support
*/


/**
 * clearRegistrations
 *
 *  Clears the entire set of registered decoders and event processors.
 *  This is intended for testing support.  All decoders and event processors must be
 *  dynamically allocated as they will be delete-d here.
 */
void
CPipelineCommand::clearRegistrations()
{
  for (pDecoder p = m_RegisteredDecoders.begin(); p != m_RegisteredDecoders.end(); p++) {
    delete p->second;
  }
  m_RegisteredDecoders.clear();
  assert(m_RegisteredDecoders.empty());

  for(pEventProcessor p = m_RegisteredEventProcessors.begin(); p != m_RegisteredEventProcessors.end(); p++) {
    delete p->second;
  }
  m_RegisteredEventProcessors.clear();
  assert(m_RegisteredEventProcessors.empty());
}
