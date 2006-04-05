///////////////////////////////////////////////////////////
//  CTreeVariableProperties.h
//  Implementation of the Class CTreeVariableProperties
//  Created on:      30-Mar-2005 11:03:54 AM
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


#if !defined(__CTREEVARIABLEPROPERTIES_H)
#define __CTREEVARIABLEPROPERTIES_H

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

// Forward definitions:

class CTreeVariable;
class CTCLInterpreter;

/**
 * Contains the actual properties of a tree variable. This will really be a struct.
 * 
 * @author Ron Fox
 * @version 1.0
 * @created 30-Mar-2005 11:03:54 AM
 */
class CTreeVariableProperties
{
  
private:
  /**
   * Name of the tcl variable that we are bound to.  Only settable on construction.
   */
  STD(string) m_sName;
  /**
   * Units associated with a Tree variable.  This is common between all tree
   * variables that are bound to this variable.
   */
  STD(string) m_sUnits;
  /**
   * A value that will be linked to the underlying Tcl variable.
   */
  double m_dValue;
  /**
   * True once the definition has changed.
   */
  bool m_fDefinitionChanged;
  /**
   * True if the value has changed and there has not yet been a firing of
   * Tcl_UpdateLinkedVar for this.
   */
  bool m_fValueChanged;
  
public:
  virtual ~CTreeVariableProperties();
  CTreeVariableProperties(STD(string) name, double value = 0.0, 
			  STD(string) units = STD(string)(""));
  CTreeVariableProperties(STD(string) name, 
			  const CTreeVariableProperties& example);
  CTreeVariableProperties& operator=(double value);
  CTreeVariableProperties& operator=(const CTreeVariableProperties& rhs);
  CTreeVariableProperties& operator=(const CTreeVariable& rhs);
  operator double();
  bool    definitionChanged();
  void    resetDefinitionChanged();
  STD(string)  getName();
  void    setUnits(STD(string) units);
  STD(string)  getUnits();
  bool    valueChanged();
  void    fireTraces();
  void    Link(CTCLInterpreter& rInterp);
  
};




#endif
