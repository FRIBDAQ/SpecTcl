// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//

/*
	Implementation file for CFit for a description of the
	class see CFit.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include <config.h>
#include "./CFit.h"    				

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


/*!
    Create an object of type CCalibFit
*/
CCalibFit::CCalibFit (): 
  m_eState(Accepting)
{ 
} 

/*!
    Called to destroy an instance of CCalibFit
*/
CCalibFit::~CCalibFit ( )
{
}
/*!
   Called to create an instance of CCalibFit that is a
   functional duplicate of another instance.

   \param rSource (const CCalibFit& ):
      The object that we will dupliate.
*/
CCalibFit::CCalibFit (const CCalibFit& aCCalibFit ) 
{
  CopyIn(aCCalibFit);

} 
/*!
  Assign to *this from rhs so that *this becomes a functional
  duplicate of rhs.
  \param rhs (const CCalibFit& rhs ):
     The object that will be functionally copied to *this.
 */
CCalibFit& CCalibFit::operator= (const CCalibFit& rhs)
{ 
  if(this != &rhs) {
    CopyIn(rhs);		// See why we bothered?
  }
  return *this;

}
/*!
  Compare *this for functional equality with another object of
  type CCalibFit.
  \param rhs (const CCalibFit& rhs ):
     The object to be compared with *this.
  \return int
  \retval 0   Not equal
  \retval !0  Equal.

 */
int 
CCalibFit::operator== (const CCalibFit& rhs) const
{
  return ((m_eState    == rhs.m_eState)   &&
	  (m_afpPoints == rhs.m_afpPoints));
}


// Functions for class CCalibFit

/*! 


Add a point to the set of points being fit.
Note that adding a point also takes a performed
fit and makes it accumulating again.


Parameters:

\param p (Point)
    The point to add. The first 'coordinate' of the point is an indepdenent parameter.
    the second, the corresponding dependent parameter.

*/
void 
CCalibFit::AddPoint(CCalibFit::Point p)  
{ 
  SetFitState(Accepting);		// This puts us back to accepting.
  m_afpPoints.push_back(p);	// Add the point.
}  

/*! 


Returns an iterator to the first point in m_afpPoints.


\return PointIterator
\return PointIterator
\retval iterator to the beginning of m_afpPoints

*/
CCalibFit::PointIterator 
CCalibFit::begin()  
{
  return m_afpPoints.begin();
}  

/*! 


Returns an iterator to support end of iteration
checking.


\return PointIterator
\retval Iterator pointing just off the end of m_afpPoints.


*/
CCalibFit::PointIterator 
CCalibFit::end()  
{
  return m_afpPoints.end();
}  

/*! 

Returns the number of points that have
been accumulated for the fit.

Parameters:


\return size_t
\retval m_afpPoints.size()

*/
size_t 
CCalibFit::size()  
{
  return m_afpPoints.size();
}  

/*! 


Returns the current state of the fit.


Parameters:

\return FitState
\return FitState
\retval accumulating - fit is accumulating points to fit.
\retval performed    - Fit has been performed, can be evaluated or queried for parameters.


*/
CCalibFit::FitState 
CCalibFit::GetState()  const
{
  return m_eState;
}  


/*!
  Copy from rhs to this.
  \param rhs (const CCalibFit&)
    The object we copy to this.
*/
void
CCalibFit::CopyIn(const CCalibFit& rhs)
{
  m_afpPoints = rhs.m_afpPoints;
  m_eState    = rhs.m_eState;
}
