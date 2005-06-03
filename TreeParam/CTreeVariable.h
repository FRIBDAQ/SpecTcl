///////////////////////////////////////////////////////////
//  CTreeVariable.h
//  Implementation of the Class CTreeVariable
//  Created on:      30-Mar-2005 11:03:52 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#if !defined(__CTREEVARIABLE_H)
#define __CTREEVARIABLE_H


#include "CTreeVariableProperties.h"

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

// Forward definitions:

class CTreeVariableProperties;
class CTCLInterpreter;

/**
 * The class the user makes which defines the characteristics of a tree variable,
 * and its binding to a Tcl variable name.
 * @author Ron Fox
 * @version 1.0
 * @created 30-Mar-2005 11:03:52 AM
 */
class CTreeVariable
{
  // Data types:
public:
  typedef STD(map)<STD(string), CTreeVariableProperties*> TreeVariableMap;
  typedef TreeVariableMap::iterator                       TreeVariableIterator;

  // Member data:
private:
  /**
   * This contains pointers to the parameter objects of all of the tree variables.
   */
  static TreeVariableMap  m_TreeVariableDictionary;
  /**
   * Points to the tree variable.
   */
  mutable CTreeVariableProperties *m_pVariable;
public:
  CTreeVariable();
  CTreeVariable(STD(string) name, double value, STD(string) units);
  CTreeVariable(CTreeVariableProperties& properties);
  ~CTreeVariable();
  void Bind();
  CTreeVariable(const CTreeVariable& rhs);
  operator double() const;
  CTreeVariable& operator=(double rhs);
  CTreeVariable& operator=(const CTreeVariable& rhs);
  CTreeVariable& operator+=(double rhs);
  CTreeVariable& operator-=(double rhs);
  CTreeVariable& operator*=(double rhs);
  CTreeVariable& operator/=(double rhs);
  double operator++(int dummy);
  CTreeVariable& operator++();
  double operator--(int dummy);
  CTreeVariable& operator--();
  STD(string) getName();
  double getValue();
  STD(string) getUnit();
  bool hasChanged();
  bool valueChanged();
  void resetChanged();
  void Initialize(STD(string) name, double value, STD(string) units);
  static void BindVariables(CTCLInterpreter& rInterp);
  static TreeVariableIterator begin();
  static TreeVariableIterator end();
  static TreeVariableIterator find(STD(string) name);
  static int size();
  
private:
  void LinkProperties(STD(string) name, STD(string) units, double value = 0.0);
  CTreeVariableProperties* getBoundVariable() const;

  friend class TreeTestSupport;
  static void CleanMap();		// For unit testing only
};




#endif 
