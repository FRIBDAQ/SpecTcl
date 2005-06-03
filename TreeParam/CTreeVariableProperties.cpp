///////////////////////////////////////////////////////////
//  CTreeVariableProperties.cpp
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

#include <config.h>
#include "CTreeVariableProperties.h"
#include "CTreeVariable.h"
#include "CTreeException.h"

#include "CTreeParameter.h"


#include <TCLInterpreter.h>
#include <TCLVariable.h>
#include <SpecTcl.h>
#include <tcl.h>
#include <TCLVersionHacks.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

CTreeVariableProperties::~CTreeVariableProperties()
{

}


/**
 * Constructs a class that has the shared tree variable properties.  The
 * constructor requires an initial value (defaults to 0.0), and a units (defaults
 * to "").  The two changed flags are set to false.
 * @param name    
 *        Name of the parameter.
 * @param value
 * @param units
 * 
 */
CTreeVariableProperties::CTreeVariableProperties(string name, double value, 
						 string units) :
  m_sName(name),
  m_sUnits(units),
  m_dValue(value),
  m_fDefinitionChanged(false),
  m_fValueChanged(false)
{    

}


/**
 * Construct a properties object from a sample... but with a different name of
 * course.
 * @param name
 *        Name of new properties.
 * @param example
 *        sample used to construct.
 * 
 */
CTreeVariableProperties::CTreeVariableProperties(string name, 
					    const CTreeVariableProperties& example) :
  m_sName(name),
  m_sUnits(example.m_sUnits),
  m_dValue(example.m_dValue),
  m_fDefinitionChanged(false),
  m_fValueChanged(false)

{  
}


/**
 * Assigns to us from a double.  the value member is updated and the
 * definition/value flags are both set true.
 * @param value
 *        New value for the parameter.
 * 
 */
CTreeVariableProperties& 
CTreeVariableProperties::operator=(double value)
{
  
  m_dValue             = value;
  m_fDefinitionChanged = true;
  m_fValueChanged      = true;
  
  return *this;
  
  
}


/**
 * Assigns to us from a CTreeVariableProperties:  the value is updated from the
 * source, the two changed flags are true.  If the units don't match an exception
 * is thrown if strict units specifications are on.
 *
 * @param rhs
 * 
 */
CTreeVariableProperties& 
CTreeVariableProperties::operator=(const CTreeVariableProperties& rhs)
{
  
  if(this != &rhs) {
  
    if(CTreeParameter::StrictTypeChecking) {
      if(rhs.m_sUnits != m_sUnits) {
	throw
	  CTreeException(CTreeException::UnitsMismatch, 
			  "CTreeVariableProperties::operator=");
      }
    }

    m_dValue             = rhs.m_dValue;
    m_fDefinitionChanged = true;
    m_fValueChanged      = true;
    
  }
  return *this;
  

}


/**
 * Assigns from a CTreeVariable:  This is really just assigning from the source
 * CTreeVariable's CTreeVaraible Properties.
 * @param rhs
 * 
 */
CTreeVariableProperties& 
CTreeVariableProperties::operator=(const CTreeVariable& rhs)
{
  
  return operator=((double)(rhs));
  
  
}


/**
 * Returns the value attribute.
 */
CTreeVariableProperties::operator double()
{

  return m_dValue;


}


/**
 * Returns the m_fDefinitionChanged flag.
 */
bool 
CTreeVariableProperties::definitionChanged()
{

  return m_fDefinitionChanged;


}


/**
 * m_fDefinitionChanged <-- false
 */
void 
CTreeVariableProperties::resetDefinitionChanged()
{

  m_fDefinitionChanged = false;


}


/**
 * Get the name of the variable this is bound to.
 */
string 
CTreeVariableProperties::getName()
{

  return m_sName;


}


/**
 * sets a new value for m_sUnits, the m_fDefinitionChanged flag >only< is sert
 * true.
 * @param units
 *        New units string.  Note at some point we need to figure out a
 *        way to invalidate the variable value when this happens.
 * 
 */
void 
CTreeVariableProperties::setUnits(string units)
{
  
  m_sUnits = units;
  
  m_fDefinitionChanged = true;


}


/**
 * returns m_sUnits
 */
string 
CTreeVariableProperties::getUnits()
{

  return  m_sUnits;


}


/**
 * Returns the value of m_fValudChanged
 */
bool 
CTreeVariableProperties::valueChanged()
{

  return m_fValueChanged;


}


/**
 * If m_fValueChanged then Tcl_UpdateLinkedVar.
 */
void 
CTreeVariableProperties::fireTraces()
{
  
  if (m_fValueChanged) {
    
    SpecTcl& api(*(SpecTcl::getInstance()));
    
    CTCLInterpreter* pInterp = api.getInterpreter();
    Tcl_Interp* pRawInterp  = pInterp->getInterpreter();
    
    Tcl_UpdateLinkedVar(pRawInterp,  (tclConstCharPtr)m_sName.c_str());
    m_fValueChanged = false;
  }
  
  
}


/**
 * Link the m_dValue field to the TCL variable with the name m_sName.
 * @param rInterp
 *        The interpreter in which the variable lives.
 * 
 */
void 
CTreeVariableProperties::Link(CTCLInterpreter& rInterp)
{
  
  CTCLVariable var(&rInterp, m_sName, false);
  double value = m_dValue;
  var.Link(&m_dValue, TCL_LINK_DOUBLE);
  m_dValue = value;		// Ensure we keep value if there's one.

}



