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

#ifndef _CANALYSISPIPELINE_H
#define _CANALYSISPIPELINE_H

#ifndef _STDLIB_H
#include <stdlib.h>		// For size_t
#ifndef _STDLIB_H
#define _STDLIB_H
#endif
#endif

#ifndef _STD_LIST
#include <list>
#ifndef _STD_LIST
#define _STD_LIST
#endif
#endif

#ifndef __STD_EXCEPTION
#include <exception>
#ifndef __STD_EXCEPTION
#define __STD_EXCEPTION
#endif
#endif

#ifndef _STD_STRING
#include <string>
#ifndef _STD_STRING
#define _STD_STRING
#endif
#endif

/**
 * @file CAnalysisPipeline.h
 * @brief Define the analysis pipeline singleton.
 */

class CDataSource;
class CDecoder;
class CEventProcessor;

/**
 * @class CAnalysisPipeline
 *
 *   The singleton object of this class implements the analysis flow of control
 *   for SpecTcl.  It provides for holding the current data source
 *   reading data from it, passing the data to an object that is capable of extracting
 *   individual items from the data and passing individual those items down the
 *   analysis pipeline.
 *   
 *  Naturally the pipeline also includes mechanisms for:
 *  - Setting the data source
 *  - Setting the object that knows how to decode top level data.
 *  - Arranging the data pipeline elements.
 */
class CAnalysisPipeline
{
  // class level data:

private:
  static CAnalysisPipeline* m_pInstance;

  // private data types:

  typedef std::list<CEventProcessor*> ProcessingPipeline;
public:
  typedef ProcessingPipeline::iterator PipelineIterator;

  // Object level data/state:

private:
  CDataSource* m_pDataSource;
  CDecoder*    m_pDecoder;
  std::string  m_decoderType;

  ProcessingPipeline m_pipeline;
  ProcessingPipeline m_analysis;

  // For singletons the constructor is private:

private:
  CAnalysisPipeline();

  // instance() makes this a singleton:

public:
  static CAnalysisPipeline* instance();


  // Object methods:

public:
  void setDataSource(CDataSource* pSource);
  void setDecoder(CDecoder* pDecoder, std::string type = std::string(""));
  std::string getDecoderType() const;
  CDecoder* getDecoder();

  void addElement(CEventProcessor* pProcessor);
  void addElement(CEventProcessor* pProcessor, PipelineIterator beforeHere);
  void addElement(CEventProcessor* pProcessor, const char* beforeHere);

  PipelineIterator begin();
  PipelineIterator end();
  PipelineIterator find(const char* pName);
  size_t size();


  void addAnalysisElement(CEventProcessor* pProcessor);
  void addAnalysisElement(CEventProcessor* pProcessor, const char* beforeHere); 

  PipelineIterator analysisBegin();
  PipelineIterator analysisEnd();
  PipelineIterator analysisFind(const char* pName);
  size_t           analysisSize();
  

  void erase(PipelineIterator here);
  void eraseAnalyzer(PipelineIterator here);

  // Event handler - 
protected:
  void onReadable(CDataSource* pSource);

  // Private utilities:

private:
  void processItem(size_t itemSize, void* pItem); // iterate on the pipeline for the item.
  void eof(CDataSource* pSource);
  PipelineIterator find_in(const char* pName, ProcessingPipeline& pipe);
  void addBefore_in(
      CEventProcessor* pProcessor, const char* pName, ProcessingPipeline& pipe, 
      const char* pipeName);

  void clearPipeline();		// for testing.

  // Other static methods:

private:
  static void sourceReadable(CDataSource* pSource, void* pClientData);
  

};
class analysis_pipeline_exception : public std::exception
{
private:
  std::string m_message;
public:
  analysis_pipeline_exception(const char* pmsg) : m_message(pmsg) {}
  virtual ~analysis_pipeline_exception() throw() {}
  virtual const char* what() const throw() { return m_message.c_str(); }
};

#endif
