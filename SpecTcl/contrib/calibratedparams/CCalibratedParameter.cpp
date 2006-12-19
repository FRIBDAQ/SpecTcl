

/*
	Implementation file for CCalibratedParameter for a description of the
	class see CCalibratedParameter.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include "CCalibratedParameter.h"    				
#include "CFit.h"

#include <Event.h>                 // Required to set/get event values.

// Design by contract provides support for precondition, postcondition,
// and invariance assertsions.
//
#include <DesignByContract.h>
using namespace DesignByContract;

// Static attribute storage and initialization for CCalibratedParameter

/*!
    Create an object of type CCalibratedParameter
    \pre The fit pointer must not be null.
    \post The source parameter id member is positive.
    \post The target parameter id member is positive.
*/
CCalibratedParameter::CCalibratedParameter (int nTargetId, int nRawId, 
									string sFitName, CFit* pFit)
   : m_nParameterId(nRawId),
   m_nTargetParameterId(nTargetId),
   m_sFitName(sFitName),
   m_pFit(0)

 
{
	REQUIRE(pFit, "Fit pointer is null!");
	m_pFit = pFit->clone(); 
	
	ENSURE(m_nParameterId >= 0, "Source parameter id negative!");
	ENSURE(m_nTargetParameterId >= 0," Target parameter id negative");
	
} 

/*!
    Called to destroy an instance of CCalibratedParameter
    \pre m_pFit is not null..
*/
 CCalibratedParameter::~CCalibratedParameter ( )
{
	REQUIRE(m_pFit, "Fit pointer became null somehow");
	delete m_pFit;
	m_pFit = (CFit*)NULL;
}
/*!
   Called to create an instance of CCalibratedParameter that is a
   functional duplicate of another instance.
   \param rSource (const CCalibratedParameter& ):
      The object that we will dupliate.
   \pre The RHS fit pointer is not null.
*/
CCalibratedParameter::CCalibratedParameter (const CCalibratedParameter& rhs) :
	m_nParameterId(rhs.m_nParameterId),
	m_nTargetParameterId(rhs.m_nTargetParameterId),
	m_sFitName(rhs.m_sFitName),
	m_pFit(0)
{
	REQUIRE(rhs.m_pFit, "RHS Fit pointer is null");
	m_pFit = rhs.m_pFit->clone();
} 
/*!
  Assign to *this from rhs so that *this becomes a functional
  duplicate of rhs.
  \param rhs (const CCalibratedParameter& rhs ):
     The object that will be functionally copied to *this.
     
  \pre RHS fit pointer is not null
 */
CCalibratedParameter& 
CCalibratedParameter::operator= (const CCalibratedParameter& rhs)
{ 
  REQUIRE(rhs.m_pFit, "RHS Fit pointer is null");
  
  if(this != &rhs) {
	m_nParameterId       = rhs.m_nParameterId;
	m_nTargetParameterId = rhs.m_nTargetParameterId;
	m_sFitName           = rhs.m_sFitName;
	delete m_pFit;
	m_pFit               = (CFit*)NULL;
	m_pFit               = rhs.m_pFit->clone();
  }
  return *this;

}
/*!
  Compare *this for functional equality with another object of
  type CCalibratedParameter.
  \param rhs (const CCalibratedParameter& rhs ):
     The object to be compared with *this.

  \pre this->m_pFit is not null.
  \pre rhs.m_pFit   is not null.
 */
int 
CCalibratedParameter::operator== (const CCalibratedParameter& rhs) const
{
	REQUIRE(m_pFit, "My Fit pointer is null");
	REQUIRE(rhs.m_pFit, "RHS Fit pointer is null");
	
	return ( 
			 (m_nParameterId       == rhs.m_nParameterId)           &&
			 (m_nTargetParameterId == rhs.m_nTargetParameterId)     &&
			 (m_sFitName           == rhs.m_sFitName)               &&
			 (*m_pFit              == *(rhs.m_pFit) )
			);
}
/*!
   Compare *this for functional inequality with another object
   of type CCalibratedParameter.  Functional inequality is defined as
   the boolean inverse of functional equality.
   \param rhs (const CCalibratedParameter& rhs ):
      The object to compare with *this.
      
   \pre Preconditions are inherited from operator== see that function
        for details.
*/
int
CCalibratedParameter::operator!= (const CCalibratedParameter& rhs) const
{
  return !(*this == rhs);
}

// Functions for class CCalibratedParameter

/*! 

Description:

Calcluates the resulting parameter 
from the corresponding raw parameter. 
Note that we assume the underlying 
parameter is an integer value.  Since
an arbitrary compression of an integer
will result in spikeness in the spectrum due to 
the discrete nature of the input function;
prior to computing the calibrated value, we
add a uniform random in the range [-0.5, 0.5]
to the input value to - undescretize it.
This is a statistically crude approach... as it
does not capture the systematic bias of the
underlying distribution of the input value
over the near range.


\pre m_pFit != (CFit*)NULL
\pre m_nParameterId is positive.
\pre m_nTargetParameterId is positive.

Parameters:

\param rEvent (CEvent&)
   The event which is being operated on.

\return void

Pseudo code:

\verbatim
If Raw parameter is valid & Fit is performedTHEN
  Calibrated paramter = m_pFit(raw parameter value)
ENDIF
\endverbatim

*/
void 
CCalibratedParameter::operator()(CEvent& rEvent) const  
{ 
	REQUIRE(m_pFit, "Fit pointer is null");
	REQUIRE(m_nParameterId >= 0, "Source parameter id negative!");
	REQUIRE(m_nTargetParameterId >= 0," Target parameter id negative");
	
	if(rEvent[m_nParameterId].isValid() && 
	   (m_pFit->GetState() == CFit::Performed)) {
	  double input = (double)(rEvent[m_nParameterId]) + 
	                           (drand48() - 0.5); // undescretize the input.
	    rEvent[m_nTargetParameterId] = (ParamType)(*m_pFit)(input);
	}
}  

/*! 

Description:

Replaces the current fit with a new fit.
The fit passed in is cloned to create a local
copy.  In this way, if the actual fit is deleted, we still
compute the parameter.

\pre  Current fit pointers is not null

\post Current fit pointer is not null, and final fit == source fit.

Parameters:

\param rNewFit (const CFit&)
   Reference to the new fit.

\return CFit*

\throw  

Pseudo code:

\verbatim
temp <- m_pFit
m_pFit = clone CFit(rNewFit)
return temp
\endverbatim

*/
CFit* 
CCalibratedParameter::ReplaceFit(CFit& rFit)  
{ 
	REQUIRE(m_pFit, "My fit pointer became null");
	
	CFit* pFit = m_pFit;
	m_pFit     = (CFit*)NULL;           // Safety.
	m_pFit     = rFit.clone();

	ENSURE(m_pFit, "My fit pointer cloned to null");
	ENSURE(*m_pFit == rFit, "Cloned fit pointer != source");
	return pFit;
}
