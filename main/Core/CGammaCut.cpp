///////////////////////////////////////////////////////////
//  CGammaCut.cpp
//  Implementation of the Class CGammaCut
//  Created on:      22-Apr-2005 12:54:51 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#include <config.h>
#include "CGammaCut.h"
#include <Event.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/**
 * destructor the parent class does everything we need, and we are just 
 * a placeholder to keep the virtual chain intact.
 */
CGammaCut::~CGammaCut()
{

}


/**
 * Constructor for a cut.  The base class does most of the checking logic and we
 * just need to add
 * the stuff that makes us gamma specific.
 * @param low
 *    The cut low limit
 * @param high
 *    The cut high limit.
 * @param rParameters
 *    A vector of parameters to check for this gate.  The gate
 *    is true if any single paramter is in the cut.  The gate as a fold operates
 *    whenever a parameter is in the gate.
 * 
 */
CGammaCut::CGammaCut(Float_t low, Float_t high, const vector<UInt_t> rParameters) :
  CCut(low, high, 0),
  m_Parameters(rParameters)
{
}


/**
 * Copy constructor.
 * @param rhs
 *     Initialization:
 * 
 * 
 */
CGammaCut::CGammaCut(const CGammaCut& rhs) :
  CCut(rhs),
  m_Parameters(rhs.m_Parameters)
{
}


/**
 * Assignment operator: Assign the base class and then our parameters.
 * @param rhs
 *    The object we assign from.
 * 
 */
const CGammaCut& 
CGammaCut::operator=(const CGammaCut& rhs)
{
  
  if (this != &rhs) {
    CCut::operator=(rhs);
    m_Parameters = rhs.m_Parameters;
  }
  return *this;
  

}


/**
 * Equivalence comparison.  Two gamma gates are equivalent if their parameter
 * lists are identical and their base classses are equivalent.
 * @param rhs
 *    THe object to compare *this to
 * 
 */
int 
CGammaCut::operator==(const CGammaCut& rhs) const
{
  
  return ((CCut::operator==(rhs))    &&
	  (m_Parameters == rhs.m_Parameters));
}


/**
 * non-equivalence comparison.  Two GammaCut objects are non-equivalent if they
 * are not-equivalent.
 * @param rhs    The object to compare *this to.
 * 
 */
int 
CGammaCut::operator!=(const CGammaCut& rhs) const
{

  return !(*this == rhs);


}


/**
 * Returns a copy of the list of parameters in the gate.
 */
std::vector<UInt_t> 
CGammaCut::getParameters() const
{
  return m_Parameters;
  
}


/**
 * True if the given event is in the gate.
 * @param rEvent
 *     The 'array' of parameters to check for in-ness.
 * 
 */
Bool_t 
CGammaCut::inGate(CEvent& rEvent)
{

  size_t  nParameters      = rEvent.size();
  size_t  nOurParameters   = m_Parameters.size();
  for (int i =0; i < nOurParameters; i++)  {
    UInt_t n  = m_Parameters[i];
    if (n < nParameters)   {
      if(rEvent[n].isValid())  {
	Float_t x = rEvent[n];
	if(inGate(x))  {
	  return kfTRUE;                     // Gate made by a parameter.
	}
      }
    }
  }
  
  return kfFALSE;
  
}

/**
 * Returns an equivalent, dynamically allocated copy of *this... the caller is
 * responsible for calling delete
 * on the copy.  The copy satisfies the condition that *(this->clone()) == *this
 * Clone is a way of virtualizing the copy construction process.
 */
CGate* 
CGammaCut::clone()
{

  return new CGammaCut(*this);
  

}


/**
 * Type of gate ("gs)"
 */
string 
CGammaCut::Type() const
{

  return string("gs");


}


/**
 * Iterator to first parameter in the gate vector.
 */
CGammaCut::parameterIterator 
CGammaCut::parameterBegin()
{

  return m_Parameters.begin();


}


/**
 * Iterator to the last parameter in the parameter set.
 */
CGammaCut::parameterIterator 
CGammaCut::parameterEnd()
{

  return m_Parameters.end();


}


/**
 * Number of paramters in the list of parameter ids.
 * 
 */
size_t 
CGammaCut::parameterSize()
{

  return m_Parameters.size();


}


