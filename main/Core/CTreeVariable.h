///////////////////////////////////////////////////////////
//  CTreeVariable.h
//  Implementation of the Class CTreeVariable
//  Created on:      30-Mar-2005 11:03:52 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//


#ifndef CTREEVARIABLE_H
#define CTREEVARIABLE_H


#include "CTreeVariableProperties.h"

#include <string>
#include <map>


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
  typedef std::map<std::string, CTreeVariableProperties*> TreeVariableMap;
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
  CTreeVariable(std::string name, double value, std::string units);
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
  std::string getName();
  double getValue();
  std::string getUnit();
  bool hasChanged();
  bool valueChanged();
  void resetChanged();
  void Initialize(std::string name, double value, std::string units);
  static void BindVariables(CTCLInterpreter& rInterp);
  static TreeVariableIterator begin();
  static TreeVariableIterator end();
  static TreeVariableIterator find(std::string name);
  static int size();
  
private:
  void LinkProperties(std::string name, std::string units, double value = 0.0);
  CTreeVariableProperties* getBoundVariable() const;

  friend class TreeTestSupport;
  static void CleanMap();		// For unit testing only
};




#endif 
