/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014-2018.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Giordano Cerizza
             Ron Fox
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/
/**
 * @file CPipelineManager.cpp
 * @brief Implement the pipeline manager.
 */

#include "CPipelineManager.h"
#include <stdexcept>
#include "EventProcessor.h"
#include "SpecTcl.h"
/**
 * The singleton instance:
 */

CPipelineManager* CPipelineManager::m_pInstance(0);

/**
 * getInstance
 *   @return CPipelineManager* - Pointer to the singleton instance.
 */
CPipelineManager*
CPipelineManager::getInstance()
{
    if (!m_pInstance) {
        m_pInstance = new CPipelineManager;
    }
    return m_pInstance;
}

/**
 * registerEventProcessor
 *    Makes an event processor instance available to be used in a pipeline.
 *
 * @param name -- name under which the event processor is listed.
 * @param CEventProcessor* pProcessor - pointer to an event processor instance
 *                to register under that names
 * @throw std::logic_error if there's already an event processor with that name.
 */
void
CPipelineManager::registerEventProcessor(
    const std::string& name, CEventProcessor* pProcessor
)
{
    // Disallow duplicates:
    
    if(m_processors.count(name)) {
        std::string msg = "Duplicate event processor name: ";
        msg += name;
        throw std::logic_error(msg);
    }
    // Add to the map:
    
    m_processors[name] = pProcessor;
    
}
/**
 * createPipeline
 *    Creates a new pipeline.
 *
 *  @param name -  name fo the  new pipeline.
 *  @throw std::logic_error - for duplicate names.
 */
void
CPipelineManager::createPipeline(const std::string& name)
{
    if (m_pipelines.count(name)) {
        std::string msg = "Duplicate event processing pipeline name: ";
        msg += name;
        throw std::logic_error(msg);
    }
    m_pipelines[name] = new CTclAnalyzer::EventProcessingPipeline;
}
/**
 * appendEventProcessor
 *   Appends an eventprocessor to an analysis pipeline.  To append to the
 *   current, use getCurrentPipelineName to know which pipeline to append to.
 *
 * @param pipename - name of the pipeline to append to
 * @param evpname  - name of the registered event processor to append.
 *
 * @throw std::logic_error if either pipename or evpname are not known.
 */
void
CPipelineManager::appendEventProcessor(
    const std::string& pipename, const std::string& evpname
)
{
    MapEventProcessingPipeline::iterator pipe = m_pipelines.find(pipename);
    MapEventProcessors::iterator         evp  = m_processors.find(evpname);
    
    // neitehr of those iterators is allowed to be end:
    
    if (pipe == m_pipelines.end()) {
        std::string msg = "Event processor pipeline: ";
        msg += pipename;
        msg += " does not exist";
        throw std::logic_error(msg);
    }
    
    if(evp == m_processors.end()) {
        std::string msg = "Event processor: " ;
        msg += evpname;
        msg += " does not exist";
        throw std::logic_error(msg);
    }
    // append the event processor at the end of the selected pipeline:
    
    pipe->second->push_back(CTclAnalyzer::PipelineElement(evp->first, evp->second));
    attachIfCurrent(pipe->second, evp->second);
}
/**
 *  insertEventProcessor
 *     Inserts an event processor in a specific point in an event processing
 *     pipeline.
 *
 *  @param pipename - name of the pipe to operate on.
 *  @param evpName  - Name of the event processor to insert.
 *  @param where    - Pipeline iterator before which the event processor will
 *                    be inserted.
 *  @throw std::logic_error - pipename or evpName don't exist.
 *
 *  @note If where isn't an iterator for the specified pipeline all hell
 *        is likely to break loose.
 */
void
CPipelineManager::insertEventProcessor(
    const std::string& pipename, const std::string& evpName,
    CTclAnalyzer::EventProcessorIterator here
)
{
    // look up the pipe and event processor they both must exist:
    
    MapEventProcessingPipeline::iterator pipe = m_pipelines.find(pipename);
    MapEventProcessors::iterator         evp  = m_processors.find(evpName);
    
    if (pipe == m_pipelines.end()) {
        std::string msg = "Event processor pipeline ";
        msg += pipename;
        msg += " does not exist.";
        throw std::logic_error(msg);
    }
    
    if (evp == m_processors.end()) {
        std::string msg = "Event processor ";
        msg += evpName;
        msg += " does not exist";
        throw std::logic_error(msg);
    }
    // Insert at here:
    
    pipe->second->insert(here, CTclAnalyzer::PipelineElement(evp->first, evp->second));
    attachIfCurrent(pipe->second, evp->second);
}
/**
 * removeEventProcessor
 *     Removes an event processor from a pipeline given its name.
 *
 *  @param pipename - name of the pipe.
 *  @param evpName  - name of the event processor.
 *  @throw std::logic_error - No such pipeline, no such event processor,
 *                     event processor isn't in the specified pipeline.
 */
void
CPipelineManager::removeEventProcessor(const std::string& pipename, const std::string& evpName)
{
    // look up the pipe and event processor they both must exist:
    
    MapEventProcessingPipeline::iterator pipe = m_pipelines.find(pipename);
    MapEventProcessors::iterator         evp  = m_processors.find(evpName);
    
    if (pipe == m_pipelines.end()) {
        std::string msg = "Event processor pipeline ";
        msg += pipename;
        msg += " does not exist.";
        throw std::logic_error(msg);
    }
    
    if (evp == m_processors.end()) {
        std::string msg = "Event processor ";
        msg += evpName;
        msg += " does not exist";
        throw std::logic_error(msg);
    }
    
    // Figure out which iterator into the event processor points at that
    // event processor and delegate to overload by iterator:
    
    for (auto p = pipe->second->begin(); p != pipe->second->end(); p++) {
        if (evp->second == p->second)  {
            removeEventProcessor(pipename, p);
            return;
        }
    }
    std::string msg = evpName;
    msg += " is not an element of ";
    msg += pipename;
    throw std::logic_error(msg);
}
/**
 * removeEventProcessor
 *    Overload that removes an event processor from a pipeline given an iterator
 *    that 'points' to that element.
 * @param pipename - name of the pipe to remove from.
 * @param here     - Iteator 'pointing' to the element to remove.
 * @note All holy hell is likely to break lose if here is an iterator into
 *       another pipeline.
 * @throw std::logic_error - pipename is not a pipeline.
 */
void
CPipelineManager::removeEventProcessor(
    const std::string& pipename, CTclAnalyzer::EventProcessorIterator here
)
{
    MapEventProcessingPipeline::iterator pipe = m_pipelines.find(pipename);
    if (pipe == m_pipelines.end()) {
        std::string msg = "Event processor pipeline ";
        msg += pipename;
        msg += " does not exist.";
        throw std::logic_error(msg);
    }
    detachIfCurrent(pipe->second, here->second);
    pipe->second->erase(here);
    
}
/**
 * setCurrentPipeline - sets the current pipeline to the named pipe.
 *
 * @param pipename.
 */
void
CPipelineManager::setCurrentPipeline(const std::string& pipename)
{
    MapEventProcessingPipeline::iterator pipe = m_pipelines.find(pipename);
    if (pipe == m_pipelines.end()) {
        std::string msg = "Event processor pipeline ";
        msg += pipename;
        msg += " does not exist.";
        throw std::logic_error(msg);
    }
    detachAll();
    m_pCurrentPipeline = pipe->second;
    m_currentPipelineName = pipe->first;
    attachAll();
}
/**
 * clonePipeline
 *    Copies a pipeline to a new one.
 *
 * @param from - name of the pipeline that's cloned
 * @param to   - name of the new pipeline.
 * @throw std::logic_error - from does not exist or to does.
 */
void
CPipelineManager::clonePipeline(const std::string& from, const std::string& to)
{
    MapEventProcessingPipeline::iterator pipe = m_pipelines.find(from);
    if (pipe == m_pipelines.end()) {
        std::string msg = "Event processor pipeline ";
        msg += from;
        msg += " does not exist.";
        throw std::logic_error(msg);
    }
    createPipeline(to);            // Throws if duplicate.
    auto pToIterator = m_pipelines.find(to);
    auto pTo = pToIterator->second;             // Pointer to the list.
    for (auto p = pipe->second->begin(); p != pipe->second->end(); p++) {
        pTo->push_back(CTclAnalyzer::PipelineElement(p->first, p->second));
    }
}

/**
 * getCurrentPipeline
 *
 * @return CTclAnalyzer::EventProcessingPipeline* - current pipeline.
*/
CTclAnalyzer::EventProcessingPipeline*
CPipelineManager::getCurrentPipeline()
{
   return m_pCurrentPipeline;
}
/**
 * getCurrentPipelineName
 *
 * @return std::string -name of the current pipeline.
 */
std::string
CPipelineManager::getCurrentPipelineName() const
{
   return m_currentPipelineName;
}
/**
 * getPipelineNames
 *    
 * @return std::vector<std::string>  - vector containing the names of all
 *     pipelines.
 */
std::vector<std::string>
CPipelineManager::getPipelineNames() const
{
   std::vector<std::string> result;
   for (auto p = m_pipelines.begin(); p != m_pipelines.end(); p++) {
       result.push_back(p->first);
   }
   
   return result;
}
/**
 * getEventProcessorNames
 *    Get the names of the registered event processors.
 *
 *   @return std::vector<std::string>
 */
std::vector<std::string>
CPipelineManager::getEventProcessorNames() const
{
    std::vector<std::string> result;
    for (auto p = m_processors.begin(); p != m_processors.end(); p++) {
        result.push_back(p->first);
    }
    
    return result;
}
/**
 * getEventProcessorsInPipeline
 *    Returns the name sof the event processors in a pipeline.
 *
 * @param pipename - name of the pipeline to check.
 * @return std::vector<std::string> Names of the event processors in the pipe.
 * @throw std::logic_error - if pipename is not the name of an event processing
 *                   pipeline.
 */
std::vector<std::string>
CPipelineManager::getEventProcessorsInPipeline(const std::string& pipename) const
{
    auto pPipe = m_pipelines.find(pipename);
    if (pPipe == m_pipelines.end()) {
        std::string msg = pipename;
        msg += "  is not the name of an event processing pipeline";
        throw std::logic_error(msg);
    }
    
    // Generate the list:
    
    std::vector<std::string> result;
    for (auto p = pPipe->second->begin(); p != pPipe->second->end(); p++) {
        result.push_back(p->first);
    }
    
    return result;
}
/**
 * lookupEventProcessor
 *    Given a pointer to an event processor returns the name of that processor.
 *
 * @param p  - pointer to the event processor.
 * @return std::string - name of that event processorl
 * @throw std::logic_error - if the event processor is not registered.
 */
std::string
CPipelineManager::lookupEventProcessor(const CEventProcessor* p) const
{
    for(auto pe = m_processors.begin(); pe != m_processors.end(); p++) {
        if (p == pe->second) return pe->first;
    }
    throw std::logic_error("CPipelineManager::lookupEventProcessor - processor not registered");
}
/**
 * pipelineCount
 *
 * @return size_t - number of pipelines that are defined.
 */
size_t
CPipelineManager::pipelineCount() const
{
    return m_pipelines.size();
}
/**
 * eventProcessorCount
 *   @return size_t - number of registered event processors.
 */
size_t
CPipelineManager::eventProcessorCount() const
{
    return m_processors.size();
}

/**
 * support for readonly iteration over the event processing pipeline
 * map...though honestly using this means we've probably got an API omission.
 */
CPipelineManager::MapEventProcessingPipeline::const_iterator
CPipelineManager::pipelineBegin() const
{
    return m_pipelines.begin();
}
CPipelineManager::MapEventProcessingPipeline::const_iterator
CPipelineManager::pipelineEnd() const
{
    return m_pipelines.end();
}

/**
 *  support for readonly iteration over the registered event processors.
 *  see comments about API completeness above.
 */
CPipelineManager::MapEventProcessors::const_iterator
CPipelineManager::processorsBegin() const
{
    return m_processors.begin();
}

CPipelineManager::MapEventProcessors::const_iterator
CPipelineManager::processorsEnd() const
{
    return m_processors.end();
}

/*----------------------------------------------------------------------------
 *  Private methods:
 *
 */

/**
 * constructor
 *   private to ensure that we are singleton:
 *   -  init m_pInstance to nullptr.
 *   -  Create an event processing pipeline named "default"
 *   -  make that one current.
*/
 CPipelineManager::CPipelineManager() :
    m_pCurrentPipeline(nullptr),
    m_currentPipelineName("")
{
    std::string defaultName("default");
    createPipeline(defaultName);
    setCurrentPipeline(defaultName);
}
/**
 * destructor
 *   We only need this for unit testing.
 *   -   Delete all pipelines.
 *   -   Delete all event processors (assuming tests dynamically allocate them)
 *   -   clear the maps.
 *   -   Clear the current pipline info (not really needed but...).
 */
CPipelineManager::~CPipelineManager()
{
    for (auto p = m_pipelines.begin(); p != m_pipelines.end(); p++) {
        delete p->second;           // EVPs get deleted next.
    }
    for (auto p = m_processors.begin(); p != m_processors.end(); p++) {
        delete p->second;
    }
    
    m_pipelines.clear();
    m_processors.clear();
    
    m_pCurrentPipeline = nullptr;
    m_currentPipelineName = "";
    m_pInstance = nullptr;               // Since this is a singleton.
}
/**
 * attachIfCurrent
 *    If a processing pipeline is current, invoke OnAttach
 *    for the event processor.  Call this after an event processor
 *    is added to a pipeline.
 *
 * @param pPipe - the pipeline to check.
 * @param pEp   - the Event processor.
 */
void
CPipelineManager::attachIfCurrent(
    CTclAnalyzer::EventProcessingPipeline* pPipe, CEventProcessor* pEp
)
{
    if(pPipe == m_pCurrentPipeline) {
        pEp->OnAttach(*(SpecTcl::getInstance()->GetAnalyzer()));
    }
}

/**
 * detachIfCurrent
 *   Same as attachIfCurrent but the OnDetach method is called.
 *   This should be called just before an event processor is removed
 *   from a pipeline.
 *
 *  @param pPipe - pointer to the pipeline to check.
 *  @param pEp   - Pointer to the event processor.
 */
void
CPipelineManager::detachIfCurrent(
    CTclAnalyzer::EventProcessingPipeline* pPipe, CEventProcessor* pEp
)
{
    if (pPipe = m_pCurrentPipeline) {
        pEp->OnDetach(*(SpecTcl::getInstance()->GetAnalyzer()));
    }
}
/**
 * detachAll
 *    Detaches all event processors in the current pipeline from
 *    the analyzer - should be called just before a new pipeline is
 *    put in as current
 * Implicit inputs: m_pCurrentPipeline
 */
void
CPipelineManager::detachAll()
{
    CAnalyzer* pAnalyzer = SpecTcl::getInstance()->GetAnalyzer();
    for (auto p = m_pCurrentPipeline->begin(); p != m_pCurrentPipeline->end(); p++) {
        p->second->OnDetach(*pAnalyzer);
    }
}
/**
 * attachAll
 *    Attaches all event processors in the current pipeline
 *    to the analyzer. Should be called right after an analyzer pipeline
 *    is made current.
 *
 *  Implicit inputs: m_pCurrentPipeline
 *  
 */
void
CPipelineManager::attachAll()
{
    CAnalyzer* pAnalyzer = SpecTcl::getInstance()->GetAnalyzer();
    for (auto p = m_pCurrentPipeline->begin(); p != m_pCurrentPipeline->end(); p++) {
        p->second->OnAttach(*pAnalyzer);        
    }
}
