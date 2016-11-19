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
#include "CFit.h"    				

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


/*!
    Create an object of type CFit
*/
CFit::CFit (string name, int id) : 
  CNamedItem(name, id),
  m_eState(Accepting)
{ 
} 

/*!
    Called to destroy an instance of CFit
*/
CFit::~CFit ( )
{
}
/*!
   Called to create an instance of CFit that is a
   functional duplicate of another instance.

   \param rSource (const CFit& ):
      The object that we will dupliate.
*/
CFit::CFit (const CFit& aCFit ) :
  CNamedItem(aCFit)
{
  CopyIn(aCFit);

} 
/*!
  Assign to *this from rhs so that *this becomes a functional
  duplicate of rhs.
  \param rhs (const CFit& rhs ):
     The object that will be functionally copied to *this.
 */
CFit& CFit::operator= (const CFit& rhs)
{ 
  if(this != &rhs) {
    CNamedItem::operator=(rhs);
    CopyIn(rhs);		// See why we bothered?
  }
  return *this;

}
/*!
  Compare *this for functional equality with another object of
  type CFit.
  \param rhs (const CFit& rhs ):
     The object to be compared with *this.
  \return int
  \retval 0   Not equal
  \retval !0  Equal.

 */
int 
CFit::operator== (const CFit& rhs) const
{
  return ((m_eState    == rhs.m_eState)   &&
	  (m_afpPoints == rhs.m_afpPoints) &&
	  CNamedItem::operator==(rhs));
}


// Functions for class CFit

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
CFit::AddPoint(CFit::Point p)  
{ 
  SetFitState(Accepting);		// This puts us back to accepting.
  m_afpPoints.push_back(p);	// Add the point.
}  

/*!
   Clear all the points in the fit. This allows a reconstitution of the
   fit with new points.. It is especially important for spectrum fits.

*/
void
CFit::ClearPoints()
{
  m_afpPoints.clear();
  SetFitState(Accepting);	// This puts us back to accepting.
}
/*! 


Returns an iterator to the first point in m_afpPoints.


\return PointIterator
\return PointIterator
\retval iterator to the beginning of m_afpPoints

*/
CFit::PointIterator 
CFit::begin()  
{
  return m_afpPoints.begin();
}  

/*! 


Returns an iterator to support end of iteration
checking.


\return PointIterator
\retval Iterator pointing just off the end of m_afpPoints.


*/
CFit::PointIterator 
CFit::end()  
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
CFit::size()  
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
CFit::FitState 
CFit::GetState()  const
{
  return m_eState;
}  


/*!
  Copy from rhs to this.
  \param rhs (const CFit&)
    The object we copy to this.
*/
void
CFit::CopyIn(const CFit& rhs)
{
  m_afpPoints = rhs.m_afpPoints;
  m_eState    = rhs.m_eState;
}
