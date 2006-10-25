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
/*
  The implementation of this class makes use of fitgsl.{c,h} included
  in SpecTcl with permission from 
  Tony Denault, and Leilehua Maly of the NASA IRTF Telescope.
*/

#include <config.h>
#include "CGaussianFit.h"
#include <DesignByContract.h>
#include <stdio.h>
#include <math.h>

#include "fitgsl.h"		// gsl gaussian fitting jacket.

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
using namespace DesignByContract;


// The template for the fit script:
//
static const char* pFitTemplate = 
"proc fitline x {\n\
   set base     %f \n\
   set height   %f \n\
   set centroid %f \n\
   set sigma    %f \n\
\n\
   set root [expr ($x - $centroid)/$sigma] \n\
   return [expr  $base + $height*exp(-0.5*($root*$root))] \n\
}\n";

/*!
  Construct the fit. while we really don't need to, we'll initialize our
  fit parameters as well as the base class.
  \param name : std::string 
     The name of the fit.
  \param id : int [0]
     The fit id, this defaults to 0.
*/
CGaussianFit::CGaussianFit(string name, int id) :
  CFit(name, id),
  m_baseline(0.0),
  m_peakHeight(0.0),
  m_centroid(0.0),
  m_sigma(0.0),
  m_chisquare(0.0)
{}

/*!
  Nothing needs to be done by us for destruction but we provide a 
  destructor in case something has to be destroyed in our base classes
*/
CGaussianFit::~CGaussianFit()
{}
/*!
  Copy construction.
  Invoke the base class copy construction and CopyIn to initialize ourselves.
  CopyIn has all the needed logic to not bother withthe fit parameters if
  the fit has not been performed.
*/
CGaussianFit::CGaussianFit(const CGaussianFit& rhs) :
  CFit(rhs)
{
  CopyIn(rhs);
}
/*!
  Assignment is essentially the same as copy construction:
*/
CGaussianFit&
CGaussianFit::operator=(const CGaussianFit& rhs)
{
  if (this != &rhs) {
    CFit::operator=(rhs);
    CopyIn(rhs);
  }
  return *this;
}
/*!
  Comparison for equality.. the base class and state must compare equal.
  if the state is Performed, then futhermore the parameters must compare.
*/
int
CGaussianFit::operator==(const CGaussianFit& rhs) const
{
  if (!CFit::operator==(rhs) || (GetState() != rhs.GetState())) return false;

  if (GetState() == CFit::Performed) {
    // Need to compare the parametersL

    return 
      (m_baseline   == rhs.m_baseline)             &&
      (m_peakHeight == rhs.m_peakHeight)           &&
      (m_centroid   == rhs.m_centroid)             &&
      (m_sigma      == rhs.m_sigma)                &&
      (m_chisquare  == rhs.m_chisquare);
  }
  return true;
}
/*!
  Inequality comparison is much simpler,  it's just the logical negation of
  equality:
*/
int
CGaussianFit::operator!=(const CGaussianFit& rhs) const
{
  return !(*this == rhs);
}

/*!
   Perform the fit.
*/
void
CGaussianFit::Perform()
{
  // We need at least 5 points to fit

  REQUIRE(size() >= 5, "At least 5 points must have been accepted");

  // Marshall the points into the data structure requiredd  by 
  // fitgsl_lm:

  fitgsl_data* pData = fitgsl_alloc_data(size());
  CFit::PointIterator p = begin();
  for (int i=0; i < size(); i++) {
    CFit::Point point = *p++;
    pData->pt[i].x = point.x;
    pData->pt[i].y = point.y;
  }
  float params[4];

  int r = fitgsl_lm(pData, params, 0);

  m_baseline   = params[FITGSL_B_INDEX];
  m_peakHeight = params[FITGSL_P_INDEX];
  m_centroid   = params[FITGSL_C_INDEX];
  m_sigma      = params[FITGSL_W_INDEX];

  m_chisquare  = ChiSquare();	// Compute our chisquare.

  fitgsl_free_data(pData);


  SetFitState(CFit::Performed);
}
/*!
  The function call operator evaluates the fit at some point
  It requires that the state of the fit be Performed.
*/
double
CGaussianFit::operator()(double x)
{
  REQUIRE(GetState() == CFit::Performed,
	  "Fit not yet performed");

  float  root   = (x-m_centroid)/m_sigma;
  return m_baseline + m_peakHeight*exp(-0.5*root*root);
}


/*!
   Get the fit parameters.
   Thes are named;
   - baseline (m_baseline).
   - height   (m_peakheight).
   - centroid (m_centroid)
   - sigma    (m_sigma)
   - chisquare (m_chisquare).
*/
CFit::FitParameterList
CGaussianFit::GetParameters()
{
  REQUIRE(GetState() == CFit::Performed,
	  "Fit not yet performed");

  CFit::FitParameterList params;
  params.push_back(CFit::FitParameter("baseline", m_baseline));
  params.push_back(CFit::FitParameter("height",   m_peakHeight));
  params.push_back(CFit::FitParameter("centroid", m_centroid));
  params.push_back(CFit::FitParameter("sigma",    m_sigma));
  params.push_back(CFit::FitParameter("chisquare",m_chisquare));
  
  return params;
}
/*!
  Return the fit type (gaussian).
*/
string
CGaussianFit::Type() const
{
  return string("gaussian");
}
/*!
   Return a string that contains a proc definition for 'fitline'
   that when passed a single numerical parameter, evaluates the fit
   at that parameter.  We require that the fit be performed.
*/
string
CGaussianFit::makeTclFitScript()
{
  REQUIRE(GetState() == CFit::Performed,
	  "Fit notyet performed");
  char scriptBuffer[1000];
  sprintf(scriptBuffer, pFitTemplate, 
	  m_baseline, m_peakHeight, m_centroid, m_sigma);

  return string(scriptBuffer);
}

/*!
     Create a dynamically instantiated clone of *this.
     This is really trivially done by using the copy constructor:
*/
CFit*
CGaussianFit::clone()
{
  return new CGaussianFit(*this);
}

/////////////////////////////////////////////////////////////////////////////
//////////////////// Private utility member functions ///////////////////////
/////////////////////////////////////////////////////////////////////////////

/*
   Copy data from rhs to this.  This is only necessary if the
   rhs has state performed.
*/
void
CGaussianFit::CopyIn(const CGaussianFit& rhs)
{
  if (rhs.GetState() == CFit::Performed) {
    m_baseline   = rhs.m_baseline;
    m_peakHeight = rhs.m_peakHeight;
    m_centroid   = rhs.m_centroid;
    m_sigma      = rhs.m_sigma;
    m_chisquare  = rhs.m_chisquare;
  }
}
/*
  Compute the chisqure goodness of fit . .this is used just
  in Perform to compute m_chisquare.  Since the gsl fit jacket we are
  using does not return the chisquare, we compute it using the
  fit function and the data points.
  For this computation, we assume:
  - sigmay(i) = sqrt(y(i)) [statistical weighting]
  - degrees of freedom = 4 (number of parameters in the fit).

  This all comes from e.g. Bevington "Data Reduction and Data Error Analysis
  for the Physical Sciences" Chapter 10.  I'm sure this is not the only
  source.
*/
float
CGaussianFit::ChiSquare()
{
  float chisquare = 0.0;		// We'll sum directly in to the member.
  CFit::PointIterator p = begin();
  for (int i =0; i < size(); i++) {
    CFit::Point pt  = *p++;
    float diff      = pt.y - (*this)(pt.x);
    chisquare      += diff*diff/pt.y;
  }
  chisquare       = chisquare/4.0;
  return chisquare;
}
