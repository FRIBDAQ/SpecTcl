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

#include "CAnalysisPipeline.h"
#include "CDataSource.h"
#include "CSpecTclInterpreter.h"
#include "CDecoder.h"
#include "CEventProcessor.h"
#include "CParameter.h"
#include <algorithm>

#include <iostream>

/**
 * @file CAnalysisPipeline.cpp
 * @brief Implements the analysis pipeline singleton object.
 */


/**
 *  The singleton instance:
 */

CAnalysisPipeline* CAnalysisPipeline::m_pInstance(0);

/**
 * Utiltity classes 
 */

/// CEventProcessor name matcher
class CNameMatch
{
private:
  std::string m_name;
public:
  CNameMatch(const char* pName) : m_name(pName) {}
  bool operator()(CEventProcessor* p) { return m_name == p->getName(); }
};

/**
 * constructor
 *   The construtor is private since this is a singleton object.
 */
CAnalysisPipeline::CAnalysisPipeline() :
  m_pDataSource(0),
  m_pDecoder(0)
{}

/// Pipeline element caller:

class CPipelineCaller
{
  void* m_pData;
  size_t m_nBytes;
  CAnalysisPipeline* m_pPipe;

public:
  CPipelineCaller(void* pData, size_t nBytes, CAnalysisPipeline* pPipe) :
    m_pData(pData), m_nBytes(nBytes), m_pPipe(pPipe)  {}

  void operator()(CEventProcessor* pProcessor) {
    if(!pProcessor->onEvent(m_pData, m_nBytes, m_pPipe)) {
      throw false;
    }
  }
};




/**
 *  instance
 *
 *   Provides access to the singleton.
 */
CAnalysisPipeline*
CAnalysisPipeline::instance()
{
  if (!m_pInstance) {
    m_pInstance = new CAnalysisPipeline;
  }
  return m_pInstance;
}

/**
 * setDataSource
 *    Sets a new data source.
 *    - If there's an existing data source it is closed and deleted.
 *    - The new data source is stored in m_pDecoder.
 *    - The new data source is asked to invoke sourceReadable when readable.
 *
 * @param pSource - The new data source.
 */
void
CAnalysisPipeline::setDataSource(CDataSource* pSource)
{
  // If there's a data source already close/delete it:

  if(m_pDataSource) {
    m_pDataSource->close();
    delete m_pDataSource;
    m_pDataSource = 0;
  }

  // Set the data source

  m_pDataSource = pSource;

  // Hook it into the event loop.

  CTCLInterpreter* pInterp = CSpecTclInterpreter::instance();
  m_pDataSource->createEvent(*pInterp, CAnalysisPipeline::sourceReadable, this);

}
/**
 * setDecoder
 *
 *    Set a new data decoder.   The decoder is responsible for parsing 'chunks'
 *    out of the data source and passing high level information about them
 *    back to the analysis pipeline which in turn passes chunks down the pipeline
 *    
 *
 * @param pDecoder  the new decoder.
 * @param type      decoder type.
 */
void
CAnalysisPipeline::setDecoder(CDecoder* pDecoder, std::string type)
{


  m_pDecoder = pDecoder;
  m_decoderType = type;
}
/**
 * getDecoderType
 *
 *   Returns the decoder type string.
 *
 * @return std::string
 */
std::string
CAnalysisPipeline::getDecoderType() const
{
  return m_decoderType;
}
/**
 * getDecoder
 *
 *  Return a pointer to the decoder or null if there isn't one.
 *
 * @return CDecoder*
 */
CDecoder*
CAnalysisPipeline::getDecoder()
{
  return m_pDecoder;
}

/**
 * addElement
 *
 *  One of several overloads by this name that add elements to the
 *  event processing pipeline.  This addElement adds the event processotr
 *  to the back of the pipeline. I would guess this is the most
 *  frequently used version of the method.
 *
 * @param pProcessor - Pointer to the element to add.  
 */
void
CAnalysisPipeline::addElement(CEventProcessor* pProcessor) {
  m_pipeline.push_back(pProcessor);
}
/**
 * addElement
 *
 *   This version of the overload adds the element just before an
 *   element 'pointed to' by an iterator.
 *
 * @param pProcessor - The processor to add.
 * @param p          - The iterator that defines where to insert.
 */
void
CAnalysisPipeline::addElement(CEventProcessor* pProcessor, CAnalysisPipeline::PipelineIterator p)
{
  m_pipeline.insert(p, pProcessor);
}
/**
 * addElement
 *    This version searches for a specific element and adds the 
 *    processing element before it.
 *
 * @param pElement - element to add to the pipeline.
 * @param pName    - Name of the element after which to add
 *                   the processor. NOTE: IF there are identically named event
 *                   processors, the first match is used.
 *
 * @throw analysis_pipeline_exception - if no event processor called pName exists.
 */
void
CAnalysisPipeline::addElement(CEventProcessor* pProcessor, const char* pName)
{
  addBefore_in(pProcessor, pName, m_pipeline, "event");

}
/**
 * begin
 *
 *  Return an iterator to the front of the pipeline.
 *
 * @return CAnalysisPipeline::PipelineIterator
 */
CAnalysisPipeline::PipelineIterator
CAnalysisPipeline::begin()
{
  return m_pipeline.begin();
}
/**
 * end
 *
 *  Return an interator to the end of the pipeline.
 *
 * @return CAnalysisPipeline::PipelineIterator
 */
CAnalysisPipeline::PipelineIterator
CAnalysisPipeline::end()
{
  return m_pipeline.end();
}
/**
 * size
 *  
 * @return  size_t number of stages in the analysis pipeline.
 */
size_t
CAnalysisPipeline::size()
{
  return m_pipeline.size();
}
/**
 * find
 *
 *  Locate an event processor by name.
 *
 * @param pName - name of the event processor.
 *  
 * @return CAnalysisPipeline::PipelineIterator - 'pointer' to the element.
 * @retval this->end() if not found.
 */
CAnalysisPipeline::PipelineIterator
CAnalysisPipeline::find(const char* pName)
{
  return find_in(pName, m_pipeline);

}
/**
 * erase
 * 
 *  Remove an element from the event pipeline.  Storage management is the 
 *  responsibility of the caller.
 *
 * @param p - Iterator that 'points' to the element to remove.
 */
void
CAnalysisPipeline::erase(CAnalysisPipeline::PipelineIterator p)
{
  m_pipeline.erase(p);
}
/**
 *  eraseAnalyzer
 *    Erase an element from the analysis pipeline.
 *    Storage management is up to the user.
 * @param p - iterator in the analysis pipeline pointing to the guy to remove.
 */
void
CAnalysisPipeline::eraseAnalyzer(CAnalysisPipeline::PipelineIterator p)
{
  m_analysis.erase(p);
}
/**
 * addAnalysisElement
 *
 *  Adds an analysis pipeline element to the end of the line.
 *
 * @param pElement - pointer to the new element to add.
 */
void
CAnalysisPipeline::addAnalysisElement(CEventProcessor* pElement) 
{
  m_analysis.push_back(pElement);
}
/**
 * addAnalysisElement CEventProcessor*, const char* -> null.
 *
 *  This overload adds an analysis element prior to a named element
 *
 *  @param pElement - the element to add.
 *  @aparam pName   - name of element to add prior to.
 *
 * @throw If pName does not exist in the pipeline.
 */
void
CAnalysisPipeline::addAnalysisElement(CEventProcessor* pElement, const char* pName)
{
  addBefore_in(pElement, pName, m_analysis, "analysis");

}

/**
 * analysisBegin
 *
 * @return PipelineIterator - 'pointer' to the front of the pipeline.
 */
CAnalysisPipeline::PipelineIterator
CAnalysisPipeline::analysisBegin()
{
  return m_analysis.begin();
}

/**
 * analysisEnd
 *
 * @return PipelineIterator - 'pointer' past the last element ofthe pipeline
 */
CAnalysisPipeline::PipelineIterator
CAnalysisPipeline::analysisEnd()
{
  return m_analysis.end();
}
/**
 * analysisSize
 *
 *  @return size_t number of elements in the analysis pipeline.
 */
size_t
CAnalysisPipeline::analysisSize() {
  return m_analysis.size();
}
/**
 * analysisFind
 *   Find an analysis pipeline element with the given name.
 * 
 * @param pName - The name to look for.
 *
 * @return PipelinIterator 
 * @retval 'pointer' to the found item 
 * @retval analysisEnd() if not found.
 */
CAnalysisPipeline::PipelineIterator
CAnalysisPipeline::analysisFind(const char* pName)
{
  return find_in(pName, m_analysis);

}

/*-----------------------------------------------------------------
** Protected methods.
*/

/**
 * onReadable
 *
 *   This is the object context handler for data source readability.
 *   We begin directing the dance between the data source, the decoder
 *   and the pipline elements:
 *    *   The decoder is informed bvia onData that data can be read.
 *    *   The decoder interacts with the data source to read a chunk
 *        of data and returns to us.
 *    *   We ask the decoder for a pointer to an item and its size.
 *    *   We  then dispatch that item to the pipeline.
 *    *   We then call the decoder's next to see if it has more items
 *        it can give us without reads (e.g. if it's buffered data).
 *        if so we go through the dispatch stuff for that as well.
 *        until next tells us we don't have anything.
 *    *   Once all this is done we return to the caller which eventually
 *        gets us back to the event loop waiting for the source to be
 *        readable again.
 * 
 *  @param pSource - Pointer to the data source (usually m_pDataSource).
 */
void
CAnalysisPipeline::onReadable(CDataSource* pSource)
{
  m_pDecoder->onData(pSource);
  do {
    size_t itemSize = m_pDecoder->getItemSize();
    void*  pItem    = m_pDecoder->getItemPointer();

    // If there are no items could be EOF or not fully read..

    if (!pItem) {
      eof(pSource);
      break;
    }

    // Reset the parameters.

    CParameter::invalidateAll();
    processItem(itemSize, pItem);

  } while (m_pDecoder->next());
}

/*-------------------------------------------------------------------
** Private methods.
*/



/**
 * processItem
 *
 *   Invokes the analysis pipeline to process an item.
 */
void
CAnalysisPipeline::processItem(size_t itemSize, void* pItem)
{
  CPipelineCaller caller(pItem, itemSize, this);
  try {
    for_each(m_pipeline.begin(), m_pipeline.end(), caller);
    for_each(m_analysis.begin(), m_analysis.end(), caller);
  }
  catch (bool failed) {
    // Pipeline cut short on failure.
  }
}
/** 
 * eof
 *   Checks for an EOF on the data source and if that's the case,
 *   closes the source.
 *
 *   @param pSource - data source.
 */
void
CAnalysisPipeline::eof(CDataSource* pSource)
{
  if (pSource->isEof()) {
    pSource->close();
  }
}
/**
 * find_in const char*, ProcessingPipeline& -> PipelineIterator
 *   Find an element in the specified pipeline
 * 
 * @param pName - Pointer to the name of the item to find.
 * @param pipe  - Reference to the pipeline that contains the element.
 *
 * @return PipelineIterator
 * @retval 'pointer' to the item found.
 * @retval pipe.end() if not found.
 */
CAnalysisPipeline::PipelineIterator
CAnalysisPipeline::find_in(
    const char* pName, CAnalysisPipeline::ProcessingPipeline& pipe)
{
  CNameMatch match(pName);
  return std::find_if(pipe.begin(), pipe.end(), match);
}
/**
 * addBefore_in CEventProcessor*, const char*, ProcessingPipeline&, const char*
 *                          -> void (can throw).
 *   Adds an element to a procesing pipeline prior to the named pipeline element.
 *
 * @param pProcessor - Processing element to add.
 * @param pName      - Name of an existing element prior to which pProcessor will be added.
 * @param pipeline   - Pipeline to add to.
 * @param pipeName   - Name of the pipeline (used to construct exception msg).
 */
void
CAnalysisPipeline::addBefore_in(
    CEventProcessor* pProcessor, const char* pName, ProcessingPipeline& pipe, 
    const char* pipeName)
{
  PipelineIterator pAfter = find_in(pName, pipe);
  if (pAfter != pipe.end()) {
    addElement(pProcessor, pAfter);
  } else {
    std::string msg(pipeName);
    msg += " pipeline has no element named : '";
    msg += pName;
    msg += "'";
    throw analysis_pipeline_exception (msg.c_str());
  }
}
			     

/**
 *  sourceReadable
 *
 *  This is the initial event handler for the data source.  It is called
 *  when data can be read without blocking from the data source.
 *  The method just establishes object context (using pClientData)
 *  and invokes the onReadalbe method
 *
 * @param pSource - Data source that is ready to be read.
 * @param pClientData - Actuall a pointer to the analysis pipeline object.
 */
void
CAnalysisPipeline::sourceReadable(CDataSource* pSource, void* pClientData)
{
  CAnalysisPipeline* pPipeline = reinterpret_cast<CAnalysisPipeline*>(pClientData);

  pPipeline->onReadable(pSource);
}

/**
 * clearPipeline
 *    Clear the event processing pipelin, deleting all event processors.
 *    This is private and therefore only intended for testing.
 */
void
CAnalysisPipeline::clearPipeline()
{
  for (PipelineIterator p = m_pipeline.begin(); p != m_pipeline.end(); p++) {
    delete *p;			// delete an element
  }
  m_pipeline.clear();		// delete all elements.

  for (PipelineIterator p = m_analysis.begin(); p != m_analysis.end(); p++) {
    delete *p;
  }
  m_analysis.clear();
}
