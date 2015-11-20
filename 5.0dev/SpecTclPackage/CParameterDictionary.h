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

#ifndef _CPARAMETERDICTIONARY_H
#define _CPARAMETERDICTIONARY_H


/**
 * @file CParameterDictionary.h
 * @brief Defines the parameter dictionary singleton class.
 */

#ifndef _CVALIDVALUE_H
#include "CValidValue.h"
#endif

#ifndef _COBSERVABLE_H
#include "CObservable.h"
#endif

#ifndef _STD_STRING
#include <string>
#ifndef _STD_STRING
#define _STD_STRING
#endif
#endif

#ifndef _STD_MAP
#include <map>
#ifndef _STD_MAP
#define _STD_MAP
#endif
#endif

#ifndef _STD_STACK
#include <stack>
#ifndef _STD_STACK
#define _STD_STACK
#endif
#endif

#ifndef _STD_LIST
#include <list>
#ifndef _STD_LIST
#define _STD_LIST
#endif
#endif

#ifndef _STD_EXCEPTION
#include <exception>
#ifndef _STD_EXCEPTION
#define _STD_EXCEPTION
#endif
#endif


/*
 * Forward definitions.
 */
class CParameter;


/**
 * Typedefs:
 */

typedef CValidValue<double> SpecTclParameter;

/**
 * @class CParameterDictionary
 *
 *  Encapsulates a dictionary of SpecTcl parameters.  Parameters are
 *  represented by a many to one mapping between CParameter objects
 *  that supply parameter meta-data and CValidValue objects that
 *  represent the actual parameter values and validity information.
 *  
 *  When a CParameter is created it registers itself with the dictionary.
 *  If necessary this creates the underlying CValidValue and associates it
 *  with the name so that the future CParameter instances with the same name 
 *  can reference the same CValidValue.
 *
 *  Note that at registration time, the CParameter object is checked against
 *  the metadata supplied by the first CParameter object of that name and
 *  exceptions are thrown if there are differences.
 *
 *  Furthermore if a CParameter object modifies its metadata it is expected to
 *  ask CParameterDictionary to update the metadata in all of the other
 *  CParameters with the same name.
 *
 *  Naturally methods to search or iterate for parameters by name are provided.
 *
 *  The parameter dictionary is observable which allows observer objects to become
 *  aware of changes in dictionary state.  The object can be queried for the change that
 *  as causing the current observation.  A stack is used to allow nested change/observations
 *  to occur though it is strongly recommended that you not use this facility.
 *
 */
class CParameterDictionary : public CObservable<CParameterDictionary>
{
  // Data definitions:

  // The parameter information:
public:
  typedef struct _ParameterInfo {
    unsigned                   s_number;
    SpecTclParameter           s_value;
    std::list<CParameter*>     s_references;

  } ParameterInfo, *pParameterInfo;

  // Types for dictionary and iteration:

private:
  typedef std::map<std::string, pParameterInfo> Dictionary;

public:
  typedef Dictionary::iterator DictionaryIterator;


  // The observer information types
public:
  typedef enum _Operation {
    addFirst, addReference, removeReference, removeLast
  } Operation;

  typedef struct _OperationInfo {
    Operation      s_op;
    std::string    s_Name;
    CParameter*    s_pParam;

  } OperationInfo, *pOperationInfo;

  typedef std::stack<OperationInfo> ObserverStack;

  // Class and object attributes:

  // The singleton instance:

private:
  static CParameterDictionary* m_pInstance;
  static unsigned              m_nNextParamNo;

  // The dictionary is a singleton so construction/destruction are private:

  // object level data:
private:
  Dictionary       m_parameters;
  ObserverStack    m_observerInfo;
  bool             m_deleting;

private:
  CParameterDictionary();
  ~CParameterDictionary();

  // The instance() method lets you get the single dictionary object.

public:
  static CParameterDictionary* instance();
  
  // Mainpulation of the dictionary:

  void add(CParameter* pParam);
  void remove(CParameter* pParam);

  // Dictionary traversal/search

  DictionaryIterator begin();
  DictionaryIterator end();
  size_t             size();
  DictionaryIterator find(std::string name);

  // Callback for observers:

  const pOperationInfo getOperation();

  // utility operations:

private:
  void observe(Operation op, CParameter* pParam);
  
  

  // Testing methods

private:
  void clearDict();
  void clearStack() {}

};

class parameter_dictionary_exception : public std::exception 
{
private:
  std::string m_msg;
public:
  parameter_dictionary_exception(const char* pMsg) throw() : m_msg(pMsg) {}
  ~parameter_dictionary_exception() throw() {}
  virtual const char* what() const throw() {return m_msg.c_str();}
};

#endif
