#ifndef __CCONSTDATA_H
#define __CCONSTDATA_H
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

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_MAP
#include <map>
#ifndef __STL_MAP
#define __STL_MAP
#endif
#endif

class CConstProcessor;

/*!
   This class contains the data used to drive the const  event processor.
   The data maintenance functions are called by the command processor or
   are available to other software given that the plugin is loaded.

   The idea is that there are two lists of parameter definitions, One for
   and parameters one for or parameters.  There's a common parameter dictionary
   that contains iterators into the appropriate list.  This allows for
   parameter deletion regardless of which list the parameter is in.

*/
class CConstData
{
  // Data type definitions:

public:
  // 
  // Parameter definition in one of the lists:
  //
  typedef struct _ParameterDefinition {
    int                s_outParameterId;        // Output parameter id.
    double             s_outValue;              // Value to stor in output parameter.
    std::vector<int>   s_inputParameters;       // Independent parameter ids.
  } ParameterDefinition;


  typedef std::vector<ParameterDefinition> ParameterDefinitions;
  typedef ParameterDefinitions::iterator   ParameterIterator;

  typedef struct _DictionaryEntry {
    ParameterDefinitions*  s_pList;
    ParameterIterator      s_pParam;
  } DictionaryEntry;

  typedef std::map<std::string, DictionaryEntry> ParameterDictionary;
  typedef ParameterDictionary::iterator    DictionaryIterator;

  // Managed data:

private:
  ParameterDefinitions    m_andParameters;
  ParameterDefinitions    m_orParameters;
  ParameterDictionary     m_dictionary;

  // Singleton instance

  static CConstData*      m_pInstance;
  static CConstProcessor* m_pProcessor;

  // Constructors...and canonicals.. remember this is a singleton:

private:
  CConstData();

  // Access to the singleton:

public:
  static CConstData& getInstance();

  // Parameter maintenance (used by the command processor typically)

public:
  void addAndParameter(std::string name, double value, std::vector<std::string> inputs);
  void addOrParameter(std::string name,  double value, std::vector<std::string> inputs);
  void deleteParameter(std::string name);

  // Iteration (used by the event processor typically)

  ParameterIterator andBegin();
  ParameterIterator andEnd();

  ParameterIterator orBegin();
  ParameterIterator orEnd();


  // Helpers and factorizers:
private:
  void addParameter(ParameterDefinitions& which, 
		    std::string name, double value, std::vector<std::string> inputs);
  void throwParameterExists(std::string name);
  void throwParameterDoesNotExist(std::string name);
  void throwParameterNotConst(std::string name);
  std::string embedName(const char* prefix, std::string name, const char* suffix);

};


#endif
