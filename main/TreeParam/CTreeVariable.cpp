//////////////////////////////////////////////////////////
//  CTreeVariable.cpp
//  Implementation of the Class CTreeVariable
//  Created on:      30-Mar-2005 11:03:53 AM
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

#include <config.h>
#include "CTreeVariable.h"
#include "CTreeVariableProperties.h"
#include "CTreeException.h"
#include <SpecTcl.h>
#include <vector>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Static data:

CTreeVariable::TreeVariableMap CTreeVariable::m_TreeVariableDictionary;


/**
 * Construct an ill defined tree variable.  No attempt is made to link the
 * variable to a properties field.  The m_pVariable field is set to null.  Usually
 * this will eventually be followed by a call to Initialize to initialize the
 * variable  properties and link it... this is essentially a no-op.
 */
CTreeVariable::CTreeVariable() :
  m_pVariable(0)
{

}


/**
 * Links us to the properties specified by the parameter's constructors (name,
 * initial value and units).
 * If the properties already exist the earlier definition overrides this.
 * @param name
 *        Name of the TCL variable we bind to.
 * @param value
 *        Initial value of the variable.
 * @param units
 *        variable's units of measure.
 * 
 */
CTreeVariable::CTreeVariable(string name, double value, string units) :
  m_pVariable(0)
{
  
  Initialize(name, value, units);


}


/**
 * Construct a tree variable given it's underlying parameter.
 * @param properties
 *        The properties objec that this tree variable will be bound
 *        to.  
 * 
 */
CTreeVariable::CTreeVariable(CTreeVariableProperties& properties) 
{

  // If these properties are not yet in the dictionary, put a dynamically
  // allocated copy of them there.

  if (m_TreeVariableDictionary.find(properties.getName()) == end()) {
    m_TreeVariableDictionary[properties.getName()]  = 
      new  CTreeVariableProperties(properties.getName(), properties);
  }
  m_pVariable = m_TreeVariableDictionary[properties.getName()];
  
}


/**
 * Destructor is a no-op since the underlying CTreeVariableProperties may be
 * linked to several CTreeVariable objects if the programmer wants this.
 */
CTreeVariable::~CTreeVariable()
{

}


/**
 * If the properties for this variable are not yet created, create them.. in any
 * event set m_pVariable to point to the properties element in the dictionary.  If
 * the tree variable properties already exist, they will not be overidden.
 * @param name
 *        Name of the tree variable.
 * @param units
 *        Tree variable units.
 * @param value
 *        Initial value of the variable.  Note that if the variable
 *        properties have already been made, they will not be overridden by us.
 * 
 */
void 
CTreeVariable::LinkProperties(string name, string units, double value)
{
  // Must make a new one...
  
  if(m_TreeVariableDictionary.find(name) == end()) {
     m_TreeVariableDictionary[name] =  
       new CTreeVariableProperties(name, value, units);
  }
  m_pVariable = m_TreeVariableDictionary[name];
}


/**
 * Returns a pointer to the bound variable, or throws an exception if m_pVariable
 * is null (not bound).
 */
CTreeVariableProperties* 
CTreeVariable::getBoundVariable() const
{
  if(! m_pVariable) {
     throw CTreeException(CTreeException::NotBound, "CTreeVariable::GetBound");
  }
  return m_pVariable;
}


/**
 * Links the underlying properties field to a Tcl variable.  Note that if it is
 * already linked it does not hurt to link it again.. that just replaces the link
 * with the one that was already there.  Throws an exception if m_pVariable is
 * NULL.
 */
void 
CTreeVariable::Bind()
{
  
  CTreeVariableProperties* pProperties = getBoundVariable();
  
  SpecTcl& api(*(SpecTcl::getInstance()));
  
  CTCLInterpreter *pInterp = api.getInterpreter();
  pProperties->Link(*pInterp);
}


/**
 * Copy constructor.   m_pVariable is set to the rhs's m_pVariable.
 * @param rhs
 * 
 */
CTreeVariable::CTreeVariable(const CTreeVariable& rhs) :
  m_pVariable(rhs.m_pVariable)
{
  // Note that we don't need any linking or binding as the state of the
  // rhs's link/bind is sufficient unto us.
}


/**
 * converts *this to a double.  If m_pVariable is NULL, throw an exception
 * otherwise, return the value of the m_pVariable.  This assumes that the properties
 * class has an operator double() as well.
 */
CTreeVariable::operator double() const
{
  CTreeVariableProperties* p = getBoundVariable();
  return (double)(*p);
}


/**
 * Assigns to this from a double.  If m_pVariable is NULL, throw an exception.
 * Otherwise, assign to *m_pVariable from the double, and return *this
 * @param rhs
 *      source of the assignment.
 */
CTreeVariable& 
CTreeVariable::operator=(double rhs)
{
  
  CTreeVariableProperties& rV(*(getBoundVariable()));
  
  rV = rhs;			// assigns only the value.
  return *this;			// But full chaining by returning *this.
  
  
}


/**
 * Assigns to this from another CTreeVariable.  If either variable's m_pVariable
 * is NULL, throw an exception.  Otherwise, assign to *m_pVariable from rhs.
 * @param rhs
 *       Source of the assignment.
 */
CTreeVariable& 
CTreeVariable::operator=(const CTreeVariable& rhs)
{
  
  *this = (double)rhs;		// This cast forces assignment of value.
  
  return *this;
  
  
}


/**
 * Adds a value to the tree variable.   *this is returned.
 * @param rhs
 *       Source of the assignment.
 */
CTreeVariable& 
CTreeVariable::operator+=(double rhs)
{

  CTreeVariableProperties& pV(*(getBoundVariable()));
  
  pV = (double)pV + rhs;
  return *this;
}


/**
 * Subtracts a value from the tree variable and returns *this.
 * @param rhs
 * 
 */
CTreeVariable& 
CTreeVariable::operator-=(double rhs)
{

  return *this += -(double)rhs;


}


/**
 * Multiplies a tree variable by a value, and returns *this.
 * @param rhs
 * 
 */
CTreeVariable& 
CTreeVariable::operator*=(double rhs)
{

  CTreeVariableProperties& p(*(getBoundVariable()));
  
  p = (double)p * rhs;
  return *this;
}


/**
 * Divides a tree variable by a value and returns *this.
 * @param rhs
 * 
 */
CTreeVariable& 
CTreeVariable::operator/=(double rhs)
{
  CTreeVariableProperties& p(*(getBoundVariable()));

  p = (double)p / rhs;
  return *this;
}


/**
 * Postfix increment  increments the value and returns the prior value.
 *  Note that operator++ is not defined in C++ for real's but since treeparameter
 ** defined it we will too.
 * @param dummy
 *       This is required to make the operation a post increment.
 * 
 */
double 
CTreeVariable::operator++(int dummy)
{
  
  CTreeVariableProperties& p(*(getBoundVariable()));
  
  double prior = p;
  p = (double)p + 1.0;
  return prior;			// Post increment semantics.
  
}


/**
 * Increments the value and returns *this.
 *  Note that operator++ is not defined in C++ for real's but since treeparameter
 * defined it we will too.
 */
CTreeVariable& 
CTreeVariable::operator++()
{
  *this += 1.0;
  return *this;
}


/**
 * Postfix decrement.  Decrements the value and returns the value prior to the
 * decrement.
 *  Note that operator-- is not defined in C++ for real's but since treeparameter
 * defined it we will too.
 * @param dummy
 *        Required to make this post decrement semantics.
 * 
 */
double 
CTreeVariable::operator--(int dummy)
{
  double prior = *this;
  *this -= 1.0;
  return prior;
}


/**
 * prefix decrement, decrements the value and returns*this.
 *  Note that operator-- is not defined in C++ for real's but since treeparameter
 * defined it we will too.
 */
CTreeVariable& 
CTreeVariable::operator--()
{
  *this -= 1.0;
  return *this;
}


/**
 * returns the name of the variable we are bound to.  throws if we are not bound
 * to a variable.
 */
string 
CTreeVariable::getName()
{
  CTreeVariableProperties& p(*(getBoundVariable()));
  return p.getName();
}


/**
 * Returns the value of the underlying variable.
 */
double 
CTreeVariable::getValue()
{
  return  (double)*this;
}


/**
 * Return the units string of the underlying variable.
 */
string 
CTreeVariable::getUnit()
{
  CTreeVariableProperties& p(*(getBoundVariable()));
  
  return p.getUnits();
}


/**
 * True if the underlying definition has changed.
 */
bool 
CTreeVariable::hasChanged()
{
  
  CTreeVariableProperties& p(*getBoundVariable());
  return p.definitionChanged();
}


/**
 * True if the underlying value has changed.
 */
bool 
CTreeVariable::valueChanged()
{
  CTreeVariableProperties& p(*getBoundVariable());
  return p.valueChanged();
}


/**
 * Resets the definition change state of the underlying parameter.
 */
void 
CTreeVariable::resetChanged()
{
  CTreeVariableProperties& p(*getBoundVariable());
  
  p.resetDefinitionChanged();
  p.fireTraces();
}


/**
 * Initializes a pre-constructed variable.
 * @param name
 * @param value
 * @param units
 * 
 */
void 
CTreeVariable::Initialize(string name, double value, string units)
{

  LinkProperties(name, units, value);
}


/**
 * Binds all variables to underlying TCL variables.
 * @param rInterp
 * 
 */
void 
CTreeVariable::BindVariables(CTCLInterpreter& rInterp)
{
  TreeVariableIterator p = begin();
  while (p != end()) {
     CTreeVariableProperties* prop = p->second;
     prop->Link(rInterp);
     p++;
  }

}


/**
 * Begin iteration iterator to the tree variable parameter dictionary.  Note the
 * iterator is to the underlying properties not to the tree variables themselves.
 * This is a bit weird but the properties have all the stuff one would really want
 * to iterate for.
 */
CTreeVariable::TreeVariableIterator 
CTreeVariable::begin()
{
  return m_TreeVariableDictionary.begin();
}


/**
 * End of iteration iterator for the CTreeVariable dictionary.  See comments to
 * begin()
 */
CTreeVariable::TreeVariableIterator 
CTreeVariable::end()
{
  
  return m_TreeVariableDictionary.end();
  

}


/**
 * Locate tree variable property by  name.
 * @param name    Name of the variable we want to locate.
 * 
 */
CTreeVariable::TreeVariableIterator 
CTreeVariable::find(string name)
{

  return m_TreeVariableDictionary.find(name);
  

}


/**
 * Returns the number of elements in the treevariable dictionary.  This is the
 * number of underlying variables.  It is possible for there to be more tree
 * variables themselves.
 */
int 
CTreeVariable::size()
{
  
  return m_TreeVariableDictionary.size();
  

}



/// This utility should only be used by e.g. unit tests.  It clears out the
//  entire map,deleting any parameters that are in it.
/// Prior to calling this, the interpreter should have been destroyed as well.
//  As otherwise there will be dangling traces and links.
//
void
CTreeVariable::CleanMap()
{
  // due to the way iterators work it's simplest to pull the items out of the map,
  // delete the items and clearn the map.
  //
  vector<pair<string, CTreeVariableProperties*> > params;
  TreeVariableIterator p = begin();
  while (p != end()) {
    params.push_back(*p);
    p++;
  }

  for(int i =0; i < params.size(); i++) {
    delete params[i].second;
    params[i].second = (CTreeVariableProperties*)NULL;
  }
  m_TreeVariableDictionary.clear(); // Erase the map.
}
