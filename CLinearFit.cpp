   

/*
	Implementation file for CLinearFit for a description of the
	class see CLinearFit.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include <config.h>

#include "./CLinearFit.h"    				
#include <DesignByContract.h>

#include <gsl/gsl_fit.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

using namespace DesignByContract;



// Static attribute storage and initialization for CCalibLinearFit

/*!
    Create an object of type CCalibLinearFit
*/
CCalibLinearFit::CCalibLinearFit ()
   : m_fSlope(0),
   m_fOffset(0),
   m_fChiSquare(0)
 
{ 
} 

/*!
    Called to destroy an instance of CCalibLinearFit
*/
 CCalibLinearFit::~CCalibLinearFit ( )
{
}
/*!
   Called to create an instance of CCalibLinearFit that is a
   functional duplicate of another instance.
   \param rSource (const CCalibLinearFit& ):
      The object that we will dupliate.
*/
CCalibLinearFit::CCalibLinearFit (const CCalibLinearFit& aCCalibLinearFit ) 
  : CCalibFit (aCCalibLinearFit),
    m_fSlope(aCCalibLinearFit.m_fSlope),
    m_fOffset(aCCalibLinearFit.m_fOffset),
    m_fChiSquare(aCCalibLinearFit.m_fChiSquare)
{
} 
/*!
  Assign to *this from rhs so that *this becomes a functional
  duplicate of rhs.

  \param rhs (const CCalibLinearFit& rhs ):
     The object that will be functionally copied to *this.
  \return CCalibLinearFit&
  \retval *this

 */
CCalibLinearFit& CCalibLinearFit::operator= (const CCalibLinearFit& rhs)
{ 
  if(this != &rhs) {
    CCalibFit::operator=(rhs);
    m_fSlope     = rhs.m_fSlope;
    m_fOffset    = rhs.m_fOffset;
    m_fChiSquare = rhs.m_fChiSquare;
  }
  return *this;

}
/*!
  Compare *this for functional equality with another object of
  type CCalibLinearFit.
  \param rhs (const CCalibLinearFit& rhs ):
     The object to be compared with *this.

 */
int 
CCalibLinearFit::operator== (const CCalibLinearFit& rhs) const
{
  if(!CCalibFit::operator==(rhs)) return false;

  // Note that state equality is presumably a requirement of
  // CCalibFit::operator==

  if(GetState() == CCalibFit::Performed) { // fit params are valid.
    return ((m_fSlope     == rhs.m_fSlope)           &&
	    (m_fOffset    == rhs.m_fOffset)          &&
	    (m_fChiSquare == rhs.m_fChiSquare));
  }
  else {
    return true;		// Members don't matter.
  }
}
  /*!
   Compare *this for functional inequality with another object
   of type CCalibLinearFit.  Functional inequality is defined as
   the boolean inverse of functional equality.
   \param rhs (const CCalibLinearFit& rhs ):
      The object to compare with *this.
*/
int
CCalibLinearFit::operator!= (const CCalibLinearFit& rhs) const
{
  return !(*this == rhs);
}

// Functions for class CCalibLinearFit

/*! 

Description:

Perform the linear least squares fit of a 
line to the data points.  The slope and
offset are stored internally in m_fSlope 
and m_fOffset respectively.  They can
be fetched via GetParameters.


Parameters:

\pre At least 2 points must have been accepted.
\pre If two points have been accepted, they are not on a line
      parallel with the y axis.

\return void
\throw PreconditionExeption
   The preconditions described above were not met.



*/
void 
CCalibLinearFit::Perform()  
{

  // Check the preconditions..

  REQUIRE(size() >= 2, "At least 2 points must have been accepted");
  if(size() ==2) {		// We're not going to check for 3..
    CCalibFit::PointIterator p = begin();
    CCalibFit::Point p1  = *p++;	// first point
    CCalibFit::Point p2  = *p;	// second point
    REQUIRE(p1.x != p2.x, "2 points are on a vertical line!!");
  }
  // Marshall the points into x/y arrays as required by GSL's fit
  // function.

  int    nPoints = size();
  double* x = new double[nPoints];
  double* y = new double[nPoints];
  CCalibFit::PointIterator p = begin();
  for(int i =0; i < nPoints; i++) {
    CCalibFit::Point point = *p++;
    x[i] = point.x;
    y[i] = point.y;
  }
  // Now we can do the fit:

  double cov0, cov1, cov2;	// We throw these away anyway.

  gsl_fit_linear(x, 1, y, 1, nPoints,
		 &m_fOffset, &m_fSlope,
		 &cov0, &cov1, &cov2, &m_fChiSquare);

  // Indicate the fit was done:

  SetFitState(CCalibFit::Performed);
  delete []x;
  delete []y;
}  

/*! ;


Evaulates the fit at a specific point


Parameters:

\param x (double)
   The point evaluated.

\pre The fit must have been performed successfully.


\return double
\retval the value of the fit at that point.

\throw PreconditionException
   The fit has not yet been sucessfully performed.



*/
double 
CCalibLinearFit::operator()(double x)  
{
  // Evaluate the precondition:

  REQUIRE(GetState() == CCalibFit::Performed,
	  "Fit not yet performed");
  return (m_fSlope*x) + m_fOffset;
}  

/*! 

Description:

Returns the current fit parameters.  The parameters
are returned in a FitParameterList.  A FitParameterList
is an STL container of Pair<string, double> where string
is a description of the parameter and double is the value. 
The strings CCalibLinearFit produces are:
- slope   - slope of the line.
- offset   - Intercept of the line with the Y axis.
- chisquare - The chi square goodness of the fit.


\pre  The fit has already been performed.


Parameters:

\return FitParameterList
\return FitParameterList - the parameters of the best fit.



*/
CCalibFit::FitParameterList 
CCalibLinearFit::GetParameters()  
{ 
  REQUIRE(GetState() == CCalibFit::Performed,
	  "Fit not yet performed!");


  CCalibFit::FitParameterList aParameters;

  aParameters.push_back(CCalibFit::FitParameter("slope"    , m_fSlope));
  aParameters.push_back(CCalibFit::FitParameter("offset"   , m_fOffset));
  aParameters.push_back(CCalibFit::FitParameter("chisquare", m_fChiSquare));

  return aParameters;
}
/*!
   Clone ourselves.  Clone provides polymorphic copy construction.
   \return CCalibFit*
   \retval A pointer to an exact duplicate of this dynamically allocated.
   
 */
 CCalibFit*
 CCalibLinearFit::clone()
 {
 	return new CCalibLinearFit(*this);
 }
