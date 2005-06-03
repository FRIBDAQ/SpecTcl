///////////////////////////////////////////////////////////
//  CGammaBand.cpp
//  Implementation of the Class CGammaBand
//  Created on:      22-Apr-2005 12:54:25 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
#include <config.h>
#include "CGammaBand.h"
#include "Gate.h"
#include <Event.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


/**
 *   The destructor does not need to do anything since vectors self destruct
 *   cleanly.
 */
CGammaBand::~CGammaBand()
{

}


/**
 * 'normal' constructor.  Requires a list of points to initialize the underlying
 * band and a set of parameter id's that we will use when checking the gate.
 * @param rPoints    Set of points that make up the band.  CBand understands what
 * it means to be 'in' the band.
 * @param rParameters    Parameters ids that are going to be checked against this
 * gate.
 * 
 */
CGammaBand::CGammaBand(const vector<FPoint>& rPoints, 
		       const ParameterList& rParameters) :
  CBand(rParameters[0], rParameters[1], rPoints),
  m_Parameters(rParameters)
{

}


/**
 * Copy constructor.
 * @param rhs
 *    The object we copy to create *this.
 * 
 */
CGammaBand::CGammaBand(const CGammaBand& rhs) :
  CBand(rhs),
  m_Parameters(rhs.m_Parameters)
{
}


/**
 * Assignment from other gamma bands
 * @param rhs
 *    The object from which we assign.
 * 
 */
CGammaBand& 
CGammaBand::operator=(const CGammaBand& rhs)
{
  
  if (this != &rhs) {
     CBand::operator=(rhs);
     m_Parameters   = rhs.m_Parameters;
  }
  return *this;
  
  
}


/**
 * Equivalence comparison.  Two gamma bands are equivalent if their underlying
 * bands are equivalent and their parameter lists are equal.
 * @param rhs
 * 
 */
int 
CGammaBand::operator==(const CGammaBand& rhs) const
{

  return ((CBand::operator==(rhs))        &&
           (m_Parameters == rhs.m_Parameters));
  

}


/**
 * Two Gamma bands are not equivalent if the equivalence operator returns false.
 * @param rhs
 * 
 */
int 
CGammaBand::operator!=(const CGammaBand& rhs) const
{
  
  return !(*this == rhs);
  
  
}


/**
 * Returns a copy of the parameter number array.
 */
CGammaBand::ParameterList
CGammaBand::getParameters() const
{
  
  return m_Parameters;
  
  
}


/**
 * Returns a faithful duplicate of *this dynamically constructed.
 */
CGate* 
CGammaBand::clone()
{

  return new CGammaBand(*this);


}


/**
 * Returns the gate type: gb.
 */
string 
CGammaBand::Type() const
{

  return string("gb");


}


/**
 * Given an event returns true if the gate is true.
 * @param rEvent    A reference to the event to check.
 * 
 */
Bool_t
CGammaBand::inGate(CEvent& crEvent)
{
  CEvent& rEvent((CEvent&)crEvent);

  size_t xSize = m_Parameters.size()-1;
  size_t ySize = m_Parameters.size();
  size_t nPar  = rEvent.size();
  
  for(int i = 0;  i < xSize; i++) {
    for(int j = i+1; j < ySize; j++) {
      UInt_t iPar   = m_Parameters[i];
      UInt_t jPar   = m_Parameters[j];
      if((iPar < nPar) && (jPar < nPar)) {
	if(rEvent[iPar].isValid()    &&  rEvent[jPar].isValid()) {
	  Float_t iv = rEvent[iPar];
	  Float_t jv = rEvent[jPar];
	  if (Inside(iv, jv)   ||  Inside(jv, iv)) { // Check symmetrically about x=y.
	    return kfTRUE;
	  }
	}
      }
    }
  }
  return kfFALSE;
  
}


/**
 * Returns a start of iteration iterator to the paramter indices.
 */
CGammaBand::parameterIterator 
CGammaBand::parametersBegin()
{

  return m_Parameters.begin();


}


/**
 * Returns an end of iteration to the parameter id list.
 */
CGammaBand::parameterIterator 
CGammaBand::parametersEnd()
{

  return m_Parameters.end();


}


/**
 * Returns the number of parameters on which the gate is set.
 */
size_t 
CGammaBand::parameterSize()
{

  return m_Parameters.size();


}


