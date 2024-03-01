/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include <config.h>        // SpecTcl configuration file.
#include "CConstData.h"    // my class.
#include "CConstProcessor.h"
#include <SpecTcl.h>       // SpecTcl API.


using namespace std;


// Class level data:

CConstData*       CConstData::m_pInstance(0); // Singleton pointer.
CConstProcessor*  CConstData::m_pProcessor(0);

static const char* ProcessorName = "ConstEventProcessor";

///////////////////////////////////////////////////////////////////////
/*
 * Constructor is private:
 */
CConstData::CConstData()
{
  m_pInstance = this;
}

/*!
   Obtain the instance pointer:
   @return CConstData&
   @retval Pointer to the single instance (which is crated if necessary).

*/
CConstData&
CConstData::getInstance()
{
  if (!m_pInstance) {
    new CConstData();		// Stores the instance pointer.
  }


  return *m_pInstance;
}

//////////////////////////////////////////////////////////////////////
/*!
   Adds an and parameter.  
   @param name (std::string)  The name of the output parameter.
   @param value (double)      The output value to assign the parameter if
                              all the independent parameters got a value this event.
   @param inputs (std::vector<std::string>)  The independent parameters.
   
   @throw string - output paramter exists.
   @throw string - One of the input parameters does not exist.
*/
void
CConstData::addAndParameter(std::string name, double value, 
			    std::vector<std::string> inputs)
{
  addParameter(m_andParameters, 
	       name, value, 
	       inputs);
}
/*!
   Adds an or parameter.
   Adds an and parameter.  
   @param name (std::string)  The name of the output parameter.
   @param value (double)      The output value to assign the parameter if
                              any of the independent parameters got a value this event.
   @param inputs (std::vector<std::string>)  The independent parameters.
   
   @throw string - output paramter exists.
   @throw string - One of the input parameters does not exist.
*/
void
CConstData:: addOrParameter(std::string name,  double value, 
			   std::vector<std::string> inputs)
{
  addParameter(m_orParameters,
	       name, 
	       value, 
	       inputs);
}

/*!
   Removes an existing parameter.
   - The parameter is removed from the appropriate list.
   - The parameter is removed from the dictionary.
   - The SpecTcl parameter is destroyed via the api.

   @param name (std::string) Name of the parameter to destroy.

   @throw string - If parameter is not a const parameter.
*/
void
CConstData::deleteParameter(std::string name)
{
  DictionaryIterator p = m_dictionary.find(name);
  if (p == m_dictionary.end()) {
    throwParameterNotConst(name);
  }
  else {
    p->second.s_pList->erase(p->second.s_pParam); // Erase from list.
    m_dictionary.erase(p);
    SpecTcl* pApi = SpecTcl::getInstance();
    pApi->RemoveParameter(name);
    
  }

}
//////////////////////////////////////////////////////////////////////



CConstData::ParameterIterator
CConstData::andBegin()
{
  return m_andParameters.begin();
}

CConstData::ParameterIterator
CConstData::andEnd()
{
  return m_andParameters.end();
}

CConstData::ParameterIterator
CConstData::orBegin()
{
  return m_orParameters.begin();
}

CConstData::ParameterIterator
CConstData::orEnd()
{
  return m_orParameters.end();
}

///////////////////////////////////////////////////////////////////////


/*
 *
 *  Adds a new parameter
 *  This is common code for the addAndParameter and 
 *  addOrParameter methods.  The call sequence is the same, however the
 *  first parameter is a reference to the list in which the parameter will be
 *  added.
 */
void
CConstData::addParameter(ParameterDefinitions& which, 
			 std::string name, double value, 
			 std::vector<std::string> inputs)
{
  SpecTcl*            pApi     = SpecTcl::getInstance();
  ParameterDefinition def;
  DictionaryEntry     dict;
  CParameter*         pParameter;

  // First validate the output and input parameters.
  // if they are not valid, call the appropriate thrower method:

  pParameter  = pApi->FindParameter(name); 
  if(pParameter) {
    throwParameterExists(name);
  }
  for (int i =0; i < inputs.size(); i++) {
    pParameter = pApi->FindParameter(inputs[i]);
    if (!pParameter) {
      throwParameterDoesNotExist(inputs[i]);
    }
    def.s_inputParameters.push_back(pParameter->getNumber());
  }
  // It's legal to make the parameter.  We already have the list of
  // input parameters.  make the output parameter:

  def.s_outParameterId = pApi->AssignParameterId();
  def.s_outValue         = value;
  pApi->AddParameter(name,def.s_outParameterId, string(""));

  which.push_back(def);

  dict.s_pList  = &which;
  dict.s_pParam = which.end() - 1;

  // Finally if the event processor is not yet registered, register it.
  // if it is registered, move it to the back of the processing list:

  if(m_pProcessor) {
    pApi->RemoveEventProcessor(string(ProcessorName));
  }
  else {
    m_pProcessor = new CConstProcessor;
  }
  pApi->AddEventProcessor(*m_pProcessor, ProcessorName);

 
}

/*
 * Throw a string exception that says a parameter exists.
 * Parameters:
 *   name - name of the  parameter.
 */
void
CConstData::throwParameterExists(std::string name)
{
  string exception = embedName("Cannot use", name,
			       "as a const because it is a previously defined parameter");
  throw exception;
}
/*
 * Throw the string exception appropriate to a nonexistent parameter.
 */
void
CConstData::throwParameterDoesNotExist(std::string name)
{
  string exception = embedName("There is no parameter named",name, 
			     "defined in SpecTcl");
  throw exception;
}
/*
 * Throw the string exception appropriate to a parameter not being an existing
 * const parameter.
 */
void
CConstData::throwParameterNotConst(std::string name)
{
  string exception =  embedName("", name,
			     "is not an existing 'const' parameter");
  throw exception;
}

/*
 * Glue three strings together separated by space:
 */
std::string
CConstData::embedName(const char* prefix, std::string name, const char* suffix)
{
  string output(prefix);
  output += ' ';
  output += name;
  output += ' ';
  output += suffix;

  return output;
}
