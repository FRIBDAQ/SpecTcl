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
#ifndef __CPIPELINECOMMAND_H
#define __CPIPELINECOMMAND_H


/**
 * @file CPiplineCommand.h
 * @brief Defines the class that handles the ::spectcl::pipeline command.
 */


#ifndef __TCLOBJECTPROCESSOR_H
#include "TCLObjectProcessor.h"
#endif

#ifndef __STD_MAP
#include <map>
#ifndef __STD_MAP
#define __STD_MAP
#endif
#endif

#ifndef _STD_EXCEPTION
#include <exception>
#ifndef __STD_EXCEPTION
#define __STD_EXCEPTION
#endif
#endif

class CDecoder;
class CEventProcessor;

/**
 * @class CPipelineCommand
 *
 *   The ::spectcl:: pipeline command manipulates the event processing pipeline
 *   This is a command ensemble with actions like
 *   *  add - adds an element to a pipeline
 *   *  remove - Removes an element from a pipeline
 *
 *  The general forms of the command are:
 *
 *  * ::spectcl::pipeline add $element  ?before $other-name? ?to $which-pipeline?
 *  * ::spectcl::pipeline remove $name ?in $which-pipeline?
 *  * ::spectcl::pipeline list ?in $which-pipeline? ?matching $pattern?
 *
 *  * ::spectcl::pipeline setdecoder $decodername
 *  * ::spectcl::pipeline showdecoder
 *
 *  In the above, items surrounded by ?'s are optional.
 *  Items beginning with $ are replaced by some actual thing.
 *
 * The add subcommand adds a new pipeline element to the end of a
 * SpecTcl pipeline:
 *
 *    * $element is a pipeline element type (this is normally related to the
 *      name of the class that implements that pipeline element.
 *    * to $which-pipline if present selects which of the pipelines to
 *      add the element to.  This can be either 'event' or 'analysis'
 *      The 'event' pipline is responsible for turning raw events into
 *      tree parameters, while the 'analysis' pipeline is responsible
 *      for doing something to the tree (e.g. histogramming, filtering etc).
 *      if not provided 'event' is the default.
 *   * If before is given, the other-name parameter is the name of an event processor
 *   * prior to which the event processor must be added in that pipeline.
 *
 * The remove subcommand removes a pipeline element.:
 *
 *  * $name -is the name of the pipeline element to remove.
 *  * from $which-pipeline if provided indicates in which pipeline the
 *    element must live.  If not provided this defaults to 'analysis'.
 *    It is an error to attempt ot remove a pipline element that does not exist.
 *
 * The list subcommand lists the names of the pipline elements in pipeline order.
 *   * in if provided restricts the pipeline to the named $which-pipelne.  If not provided,
 *     both pipelines are listed.
 *   * ?matching if provided restricts the listed elements to those whose names
 *     match $pattern which is a glob pattern.
 *
 *  On successful completion, this command returns a Tcl list.  Each element of the list
 *  contains in order: 
 *   * The pipeline name ('analysis' or 'event')
 *   * The names of the pipeline elements that match the pattern in the order
 *     in which they live in the pipeline.
 *
 * The setdecoder subcommand sets the current data decoder item for the pipeline.
 *
 * @note this is a singleton object.
 */
class CPipelineCommand :   public CTCLObjectProcessor
{
  // Private data types:
private:
  typedef std::map<std::string, CDecoder*>        Decoders;
  typedef Decoders::iterator                      pDecoder;

  typedef std::map<std::string, CEventProcessor*> EventProcessors;
  typedef EventProcessors::iterator               pEventProcessor;

  typedef struct _KeywordData {
    bool        present;
    std::string value;
    _KeywordData() : present(false) {}
    _KeywordData(std::string defaultValue) : present(false), value(defaultValue) {}
  } KeywordData, *pKeywordData;
  typedef std::map<std::string, KeywordData> Keywords; 
  typedef Keywords::iterator KeywordIterator;

private:
  // Class level data

  static CPipelineCommand*  m_pInstance;

  // Object level data:
private:

  EventProcessors m_RegisteredEventProcessors;
  Decoders        m_RegisteredDecoders;


  // Canonicals:

private:
  CPipelineCommand();
  ~CPipelineCommand();

public:
  static CPipelineCommand* instance();
  
  /* 
     Externally visible interfaces.   

  */
public:
  virtual int operator() (CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

  // Decoder factory management

  void       defineDecoder(std::string decoderType, CDecoder* pDecoder);
  CDecoder*  removeDecoder(std::string decoderType);

  // Pipeline element factory management:

  void             defineElement( CEventProcessor* pProcessor);
  CEventProcessor* removeElementDefinition(std::string elementType);

  // These actually do something, operator() dispatches to them:

private:
  void addElement(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void removeElement(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void listElements(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

  void setDecoder(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void showDecoder(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

  // Utilities

private:
  size_t eventProcessorsSize();
  pEventProcessor findEventProcessor(std::string type);

  size_t decodersSize() const;
  pDecoder findDecoder(std::string type);
  void     processKeywords(std::vector<CTCLObject>& objv, unsigned startingAt,
			   Keywords& keywordMap);
  std::string keylist(Keywords& keywordMap);


  // Testing support.

  void clearRegistrations();
};
//

class pipelinecommand_exception : public std::exception
{
private:
  std::string m_message;
public:
  pipelinecommand_exception(const char* pmsg) : m_message(pmsg) {}
  virtual ~pipelinecommand_exception() throw() {}
  virtual const char* what() const throw()  { return m_message.c_str(); }
};

#endif
