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
#ifndef PARAMETERDECODING_H
#define PARAMETERDECODING_H
/** @file:  ParameterDecoding.h
 *  @brief: Class to decode parameter data files.  The idea is that when
 *          any of the ring buffer decoders  see a ring item that's described
 *          by AnalysisRingItems.h it can dispatch here for processing.
 *          
 */
#include "AnalysisRingItems.h"
#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <Event.h>
#include <EventList.h>

class CTreeParameter;


namespace spectcl {                 // Long overdue to put SpecTcl classes in a namespace.
    

    /**
     * @class ParameterDecoder
     *
     *   This class is invoked by the ring item decoders whenever they see
     *   a Type id that's above the NSCLDAQ's last type id.  If the resulting
     *   type id is something that might have come from the FRIB analysis pipeline,
     *   it is processed.  If not, we let the caller know.
     *
     *   The class maintains internal data:
     *   -    Mappings between parameter numbers and SpecTcl parameters.
     *   -    Information about the variables definitions/values as well as
     *        methods to allow access to them.
     *   -    An observer that is called for unrecognized item types.
     *        This is a hook that, in the future, can be used to support processing
     *        of user data types.
     *        
     * 
     */
    class ParameterDecoder {
    public:
        
        // Public type definitions:
        
        // Observer interface definition
        
        class Observer {
        public:
            virtual bool operator()(ParameterDecoder& decoder, const void* pItem)  = 0;
        };
        // Private data types:
        // Variables in VariableItem have this information.
        
        typedef struct _VariableDefinition {
            std::string s_units;
            double      s_value;
        } VariableDefinition, *pVariableDefinition;
        
        // Correspondence between variable names and definitions:
    private:
        typedef std::map<std::string, VariableDefinition> VariableDictionary;
        
        //  Mapping between parameter numbers and internal Parameters
        typedef std::vector<CTreeParameter*> ParameterMap;
        
        // Instance data:
    private:
        Observer*    m_pObserver;
        ParameterMap m_parameterMap;
        VariableDictionary m_variableDict;
        CEventList m_el;
        
    public:
        ParameterDecoder();
        virtual ~ParameterDecoder();
        
        // Forbidden canonicals:    
    private:
        ParameterDecoder(const ParameterDecoder& rhs);
        ParameterDecoder& operator=(const ParameterDecoder& rhs);
        int operator==(const ParameterDecoder& rhs);
        int operator!=(const ParameterDecoder& rhs);
    public:
        Observer* setObserver(Observer* newObs);
        const VariableDefinition* getVariableDefinition(const char* pName);
        bool operator()(const void* pItem);
        
    protected:
        void processParameterDefs(const frib::analysis::ParameterDefinitions* defs);
        void processParameterItem(const frib::analysis::ParameterItem* params);
        void processVariableDefs(const frib::analysis::VariableItem*   pvars);
    
    };
}
#endif
