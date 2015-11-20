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

#include "CParameterDictionary.h"
#include "CParameter.h"
#include <algorithm>
/**
 * @file CParamterDictionary.cpp
 * @brief Implement the SpecTcl parameter dictionary singleton.
 */


/* Static data */

CParameterDictionary* CParameterDictionary::m_pInstance(0);
unsigned CParameterDictionary::m_nNextParamNo(0);


/**
 * constructor
 *   no-op.
 */
CParameterDictionary::CParameterDictionary() : m_deleting(false)
{}
/**
 * destructor
 *   This is only implemented for the sake of unit tests in
 *   the application it will never get invoked.
 */
CParameterDictionary::~CParameterDictionary()
{
  m_nNextParamNo = 0;
  m_deleting = true;
  clearDict();
}

/**
 * instance
 *    Return a pointer to the singleton instance,
 *    creating it if need be.
 *
 * @return CParameterDictionary*  - pointer to the singleton.
 */
CParameterDictionary*
CParameterDictionary::instance()
{
  if (!m_pInstance) {
    m_pInstance = new CParameterDictionary;
  }
  return m_pInstance;
}



/**
 * add
 *   Add a parameter to the dictionary:
 *   - If the parameter name does not exist we create a ParameterInfo
 *     element and add it to the dict under the parameter's name.
 *   - We add the parameter to the list of references to the value.
 *   - The parameter is bound to the underlying value.
 *
 * @param pParam - pointer to a CParameter object.
 *
 */
void
CParameterDictionary::add(CParameter* pParam)
{
  std::string name = pParam->getName();
  pParameterInfo pInfo;
  DictionaryIterator pItem = m_parameters.find(name);

  // if necessary create a new parameter info struct otherwise
  // ocate it.

  if (pItem == m_parameters.end()) {
    pInfo = new ParameterInfo;
    pInfo->s_number = m_nNextParamNo++;
    m_parameters[name] = pInfo;
    observe(addFirst, pParam);
  } else {
    pInfo = pItem->second;
  }
  /*
    If the list of references is not empty we need to ensure
    this parameter matches the ones in the list.  It's sufficient
    to check against the list front since nothing gets added unless
    it's first or already added.
  */
  if (pInfo->s_references.size()) {
    CParameter* pTemplate = pInfo->s_references.front();
    if (!pParam->identical(*pTemplate)) {
      throw parameter_dictionary_exception("parameter mappings to the same value must be identical");
    }
    observe(addReference, pParam);
  }
  
  // Add our parameter as a reference to the value:

  pInfo->s_references.push_back(pParam);

  // Bind the parameter to the underlying value.

  pParam->setValue(&(pInfo->s_value));

}
/**
 * remove
 *
 *   Remove a CParameter from the dictionary.
 *   - Locate the parameter in the dictionary by name
 *   - If not found throw an exception.
 *   - If not the last parameter mapped to the value, 
 *     just remove it from the references list.
 *   - If the last parameter, free the info and remove
 *     the dictionary entry. That basically destroys the
 *     parameter completely.
 *
 * @param *pParam - Pointer to the CParameter to remove.
 */
void
CParameterDictionary::remove(CParameter* pParam) 
{
  DictionaryIterator pItem = m_parameters.find(pParam->getName());
  
  // Throw an exception if the parameter does not exist:

  if (pItem == m_parameters.end()) {
    throw parameter_dictionary_exception("no such parameter");
  }
  pParameterInfo     pInfo = pItem->second;

  // Find our item and remove it:

  std::list<CParameter*>::iterator pReference = 
    std::find( pInfo->s_references.begin(), pInfo->s_references.end(), pParam);

  // If there's no such reference throw.

  if (pReference == pInfo->s_references.end()) {
    throw parameter_dictionary_exception("not a valid reference");
  }

  // Remove the reference fromthe list of refs.

  pInfo->s_references.erase(pReference);

  // If there are no more references destroy this parameter totally:

  if (pInfo->s_references.empty()) {
    delete pInfo;
    m_parameters.erase(pItem);
    observe(removeLast, pParam);
  } else {
    observe(removeReference, pParam);
  }
}
/**
 * begin
 *   Return an iterator to the front of the parameter dictionary.
 * @return CParameterDictionary::DictionaryIterator
 */
CParameterDictionary::DictionaryIterator
CParameterDictionary::begin()
{
  return m_parameters.begin();
}
/**
 * end
 *   Return an iterator to the end of the parameter dictionary.
 * @return CParameterDictionary::DictionaryIterator
 */
CParameterDictionary::DictionaryIterator
CParameterDictionary::end()
{
  return m_parameters.end();
}
/**
 * size
 *   Returns the number of parameter names in the dictionary.
 *  
 * @return size_t
 * @note Due to the many to one mapping between CParameter and 
 *       SpecTclParameter objects there can be more CParameters than
 *       this method returns.
 */
size_t
CParameterDictionary::size()
{
  return m_parameters.size();
}
/**
 * find
 *   Returns an iterator to the dictionary entry that is for the
 *   specified string.
 *
 * @param name  - The name of the parameter to find.
 * @return CParameterDictionary::DictionaryIterator
 * @retval end() if not found.
 */
CParameterDictionary::DictionaryIterator
CParameterDictionary::find(std::string name)
{
  return m_parameters.find(name);
}

/**
 * getOperation
 *
 *  Returns a pointer to information about the operation being observed.
 *
 * @return const pOperationInfo
 */
const CParameterDictionary::pOperationInfo
CParameterDictionary::getOperation()
{
  if (!m_observerInfo.empty()) {
    return &(m_observerInfo.top());
  } else {
    return 0;
  }
}
/*-----------------------------------------------
 * Private methods.
 */

/**
 * observe 
 *   trigger an observation:
 *   - Push information about the observation on the stack.
 *   - invoke the base class observe method.
 *   - pop the observation info off the stack.
 *
 * @param op - The operation that triggered the observation.
 * @param pParam - Pointer to the parameter whose operation trggered observation.
 * @note For remove methods, the observation is done after the parameter has been
 *       removed as a reference to the value.  removeLast is done after the
 *       named parameter has been removed from the dictionary.
 *
 */
void
CParameterDictionary::observe(CParameterDictionary::Operation op, CParameter* pParam)
{
  // If we are deleting,in tests observe is not safe:

  if (!m_deleting)  {

    OperationInfo info= {op, pParam->getName(), pParam};
    m_observerInfo.push(info);
  
    CObservable<CParameterDictionary>::observe();
    
    m_observerInfo.pop();
  }
}

/*------------------------------------------------
 *  Support for testing.
 */

/**
 * clearDict
 *    Clear the dictionary and assume that all paramersare dynamically
 *    created and clear them too.
 */
void
CParameterDictionary::clearDict()
{
  while (!m_parameters.empty()) {
    DictionaryIterator pItem = m_parameters.begin();
    std::list<CParameter*>::iterator pParam = pItem->second->s_references.begin();
    remove(*pParam);
  }
  
}
