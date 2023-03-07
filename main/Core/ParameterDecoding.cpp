/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  ParameterDecoding.cpp
 *  @brief: Implement the ParameterDecoding class.
 */

#include "ParameterDecoding.h"
#include <CTreeParameter.h>
#include <SpecTcl.h>



namespace spectcl {
/**
 * Construction
 */
ParameterDecoder::ParameterDecoder() :
    m_pObserver(nullptr),
    m_el(1)
{}
 /**
  * destructor
  */
 ParameterDecoder::~ParameterDecoder() {
    // Need to kill off all of the tree parameters we made.
    
    for (auto& p : m_parameterMap) {
        delete p;
    }
 }
 /**
  * setObserver
  *    @param newObs - new observer object this object's storage is managed
  *           by the caller.
  *    @return Observer* -The prior observer
  *    @retval nullptr if no prior observer
  *    @note A well behaved observer use will have the new observer save the pointer
  *          returned from this call and, when invoked if unable to handle the
  *          item type, and if the returned value was not null, will call that
  *          observer.
  */
 ParameterDecoder::Observer*
 ParameterDecoder::setObserver(Observer* newObs) {
    auto pOld = m_pObserver;
    m_pObserver = newObs;
    return pOld;
 }
 /**
  * getVariableDefinition
  *    Return a pointer to the variable definition specified by name.
  *  @param pName - Name of the variable to lookup.
  *  @return VariableDefinition* - pointer to the definition.
  *  @retval nullptr if there is no match in the saved parameters.
  */
 const ParameterDecoder::VariableDefinition*
 ParameterDecoder::getVariableDefinition(const char* pName) {
    std::string name = pName;
    auto p = m_variableDict.find(name);
    if (p != m_variableDict.end()) {
        return &(p->second);
    } else {
        return nullptr;
    }
 }
 /**
  * function call operator
  *    Process an item type.   If the item is one of
  *    PARAMETER_DEFINITIONS, VARIABLE_VALUES or PARAMETER_DATA we invoke the
  *    appropriate protected processor and return true.  If not, we invoke the
  *    observer and return what it returns.
  *
  * @param pItem - ring item the ring item decoder did not recognize.
  * @return bool - if the item could be processed by us.
  */
 bool
 ParameterDecoder::operator()(const void* pItem) {
    const frib::analysis::RingItemHeader* pHeader =
        reinterpret_cast<const frib::analysis::RingItemHeader*>(pItem);
    switch(pHeader->s_type) {
    case frib::analysis::PARAMETER_DEFINITIONS:
        {
            const frib::analysis::ParameterDefinitions* pDefs =
                reinterpret_cast<const frib::analysis::ParameterDefinitions*>(pItem);
            processParameterDefs(pDefs);
        }
        return true;
    case frib::analysis::VARIABLE_VALUES:
        {
            const frib::analysis::VariableItem* pDefs =
                reinterpret_cast<const frib::analysis::VariableItem*>(pItem);
            processVariableDefs(pDefs);    
        }
        return true;
    case frib::analysis::PARAMETER_DATA:
        {
            const frib::analysis::ParameterItem* pData =
                reinterpret_cast<const frib::analysis::ParameterItem*>(pItem);
            processParameterItem(pData);
        }
        return true;
    default:
        if (m_pObserver) {
            return (*m_pObserver)(*this, pItem);
        } else {
            return false;
        }
    }
    
 }
//////////////////////////////////////////////////////////////////////////////
//  Internal (protected methods)
 
/**
 * processParameterDefs
 *    We process a parameter definition ring item.  This ring item
 *    allows us to create a correspondence between parameter ids used in the
 *    data source and parameters in the program.   These are maintained in a
 *    vector which contains either nulls for ids with no local parameter definition
 *    or CTreeParameter* for those that do.  Note that our computations are
 *    cumulative..if we receive more than one parameter defintion item.
 *    If a defintion changes; naturally we delete the old item first.
 *
 *    The m_parameterMap is what's built up by this. We take advantage of the
 *    fact that tree parameters with the same name map to the same underlying
 *    parameter id and metadata.
 *    
 * @param defs - pointer to a frib::analysis::ParamDefinitions ring item.
 */
void
ParameterDecoder::processParameterDefs(
   const frib::analysis::ParameterDefinitions* pDefs
) {
        auto n = pDefs->s_numParameters;
        auto p      = pDefs->s_parameters;
        
        for (int i =0; i < n; i++) {
            auto id = p->s_parameterNumber;
            std::string name = p->s_parameterName;
            CTreeParameter* par = new CTreeParameter(name);
            if (id >= m_parameterMap.size()) {
                m_parameterMap.resize(id+1, nullptr);
            }
            delete m_parameterMap[id];
            m_parameterMap[id] = par;
            
            // Now to the next parameter:
            
            p = reinterpret_cast<const frib::analysis::ParameterDefinition*>(
                &(p->s_parameterName[name.size()+1])   // past the null terminator.
            );
        }
        CTreeParameter::BindParameters();              // Bind all parameters to event indices.
}
/**
 * processParameterItem
 *    Process an item that has parameter data.  Parameter data are pairs of
 *    id/values. For each of those pairs, if there's a non-null
 *    CTreeParameter* in m_parameterMap for the id, we set it from the value.
 *    (note we also deal with parameter ids out of range).
 *
 *  @param params pointer ot the ring itemk.
 */
void
ParameterDecoder::processParameterItem(const frib::analysis::ParameterItem* params) {
    // These two lines make the tree parameters mean something.
    
    
    CTreeParameter::setEvent(*m_el[0]);
    
    for (int i  = 0; i < params->s_parameterCount; i++) {
        auto id = params->s_parameters[i].s_number;
        if (id < m_parameterMap.size() && (m_parameterMap[i] != nullptr)) {
            *m_parameterMap[i] = params->s_parameters[i].s_value;
        }
    }
    // Now that the parameters were loaded from the event, bypass the
    // analysis pipeline and invoke the data sink pipeline so that
    // the event can be histogrammed.
    
    auto pipeline = SpecTcl::getInstance()->GetEventSinkPipeline();
    (*pipeline)(m_el);
    
    // Invalidate all the parameters:
    
    m_el[0]->clear();
   
}
/**
 *  processVariableDefs
 *     Process a variable defintion ring item.  This is used to fill in the
 *     m_variableDict map which is indexed by variable name and contains
 *     the units and value of the variable from the data source.
 *
 *  @param pvars - pointer to the variable definition ring item.
 */
void
ParameterDecoder::processVariableDefs(const frib::analysis::VariableItem*   pvars) {
    auto n = pvars->s_numVars;
    auto p = pvars->s_variables;
    
    
    
    for (int i = 0; i < n; i++) {
        VariableDefinition def;
        def.s_units = p->s_variableUnits;
        def.s_value = p->s_value;
        std::string name = p->s_variableName;
        
        m_variableDict[name] = def;      // Replaces any other one.
        p = reinterpret_cast<const frib::analysis::Variable*>(
            &(p->s_variableName[name.size()+1])
        );
    }
}

}