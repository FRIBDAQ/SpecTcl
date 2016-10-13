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
#include "CSpectrumFit.h"
#include "CFit.h"
#include <SpecTcl.h>
#include <Spectrum.h>
#include <RangeError.h>
#include <DictionaryException.h>
#include <SpectrumFactoryException.h>


#include <DesignByContract.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

using namespace DesignByContract;



/*!
   Construct a Spectrum fit out of whole cloth.
   \param spectrumName  : std::string
      Name of the spectrum on which the fit is being defined.
      The spectrum must exist in SpecTcl and have dimensionality 1.
      This is the least restrictive condition and allows fits on e.g. Gamma or
      trendline spectra.
   \param myId          : int
      Id of the fit.  For use with SpecTcl/Xamine display bindings of the fitline,
      this should be a unique id in the same id namespace as gates.
   \param low, high     : int
      The region of interest over which the fit is done.  These values are channel
      numbers and must be within the raw channel numbers of the spectra.
      If you want to use axis units, it's up to some external guy to convert these
      and the fit parameters when done.
   \param fit : CFit&
      Reference to the fit to create.  This fit carries the fit type
      (e.g. linear of gaussian) in its dynamic type.

      Exceptions can be thrown which will abort the construction:

    \throw CRangeError - low or high are outside the spectrum channel limits.
    \throw CDictionaryException - The spectrum name cannot be found.
    \throw CSpectrumFactoryException(keBadSpectrumType) - The spectrum passed in
               was found but does not have dimensionality 1.

*/
CSpectrumFit::CSpectrumFit(string spectrumName, int myId,
			   int low, int high, 
			   CFit&    fit) :
  CNamedItem(spectrumName, myId),
  m_low(low),
  m_high(high),
  m_ownFit(false),
  m_pFit(&fit)
{
  // now invoke update to get the spectrum, perform the fit etc.

  update();			// That's the guy that really throws exceptions.
}
/*!
   Copy construction.  This is just a matter of copying our base class, low/high
   and cloning our fit.  This results in m_ownFit true.
   See the initial constructor for exceptions.
*/
CSpectrumFit::CSpectrumFit(const CSpectrumFit& rhs) : 
  CNamedItem(rhs),
  m_low(rhs.m_low),
  m_high(rhs.m_high),
  m_ownFit(true),
  m_pFit(rhs.m_pFit->clone())
{
  update();
}
/*!
  Destruction... if m_ownFit is true we need to delete m_pFit.
*/
CSpectrumFit::~CSpectrumFit()
{
  if (m_ownFit) {
    delete m_pFit;
  }
}
/*! 
  Assignment is similar to copy construction.  
  See the initial constructor for exceptions that can be thrown.
*/
CSpectrumFit&
CSpectrumFit::operator=(const CSpectrumFit& rhs)
{
  if (&rhs != this) {
    if (m_ownFit) delete m_pFit; // Get rid of dynamically created fit.
    CNamedItem::operator=(rhs);
    m_low    = rhs.m_low;
    m_high   = rhs.m_high;
    m_ownFit = true;
    m_pFit   = rhs.m_pFit->clone();

    update();
  }
  return *this;
}

/*!
  Comparison for equality is comparison of all our elements.
*/
int
CSpectrumFit::operator==(const CSpectrumFit& rhs) const
{
  return (CNamedItem::operator==(rhs)               &&
	  (m_low    == rhs.m_low)                   &&
	  (m_high   == rhs.m_high)                  &&
	  ((*m_pFit) == *(rhs.m_pFit)));// don't care about dynamic-ness of fit.
}
/*!
  Comparison for inequality is the logical negation of equality above.
*/
int
CSpectrumFit::operator!=(const CSpectrumFit& rhs) const
{
  return !(*this == rhs);
}

//////////////////////////////////////////////////////////////////////////
//////////////// Functions that delegate to the fit object ///////////////
//////////////////////////////////////////////////////////////////////////


/*!
  Start of iteration over points in the fit.
*/
CFit::PointIterator 
CSpectrumFit::begin()
{
  return m_pFit->begin();
}
/*!
  End of iteration over points in the fit.
*/
CFit::PointIterator 
CSpectrumFit::end()
{
  return  m_pFit->end();
}
/*!
   Number of points in the fit:
*/
size_t
CSpectrumFit::size()
{
  return m_pFit->size();
}
/*!
    Get the fit state.. note that this should really always be performed due to
    how and when we call update().
*/
CFit::FitState
CSpectrumFit::GetState()
{
  return  m_pFit->GetState();
}
/*!
   Evaluate the fit at a point.
*/
double
CSpectrumFit::operator()(double x)
{
  return (*m_pFit)(x);
}
/*!
    Get the fit parameters.
*/
CFit::FitParameterList
CSpectrumFit::getParameters()
{
  return m_pFit->GetParameters();
}
/*!
   Return the textual type of the fit e.g. 'gaussian'.
*/
string
CSpectrumFit::fitType()
{
  return m_pFit->Type();
}
/*!
    Create a tcl script that consists of a proc named fitline that can
    be evaluated for channel numbers to produce the height of the fit
    over the ROI.
*/
string
CSpectrumFit::makeTclFitScript()
{
  return m_pFit->makeTclFitScript();
}
/*!
   Return the name of the fit.  This distinct from our name which can be gotten
   by getName().. this is the name of m_pFit.
*/
string
CSpectrumFit::fitName()
{
  return m_pFit->getName();
}
/*!
   Clone ourselves.  This is essentially copy construction.
*/
CSpectrumFit* 
CSpectrumFit::clone()
{
  return new CSpectrumFit(*this);
}
/*!
  Update the fit:
  - Locate the spectrum
  - Validate the spectrum dimensionality.
  - validate low/high limits.
  - Clear the fit points.
  - Add points to the fit for each channel in the roi [m_low, m_high].
  - Perform the fit.
*/
void
CSpectrumFit::update()
{
  CSpectrum* pSpectrum = findSpectrum();
  if (!pSpectrum) {
    throw CDictionaryException(static_cast<Int_t>(CDictionaryException::knNoSuchKey),
			       "Locating spectrum in CSpectrumFit::update()",
			       getName());
  }
  // Ensure the spectrum has dimensionality 1:

  if (pSpectrum->Dimensionality() != 1) {
    // not a 1d.
    throw CSpectrumFactoryException(pSpectrum->StorageType(),
				    pSpectrum->getSpectrumType(),
				    getName(),
				    CSpectrumFactoryException::keBadSpectrumType,
				    "CSpectrumFit::update() - fits can only be done on spectra with dimensionality 1");
  }
  Size_t channelCount = pSpectrum->Dimension(0);
  if ((m_low < 0) || (m_low >= channelCount)) {
    throw CRangeError(0, channelCount, m_low,
		      "CSpectrumFit::update - fit ROI low limit out of range");
  }
  if ((m_high < 0) || (m_high >= channelCount)) {
    throw CRangeError(0, channelCount, m_low,
		      "CSpectrumFit::update - fit ROI high limit is out of range");
  }
  // Now compute the new fit:

  m_pFit->ClearPoints();
  for(UInt_t i = m_low; i<= m_high; i++) {
    CFit::Point pt;
    pt.x  = (double)i;
    pt.y  = (double)((*pSpectrum)[&i]);
    m_pFit->AddPoint(pt);
  }
  m_pFit->Perform();


}

/*!
  Select the low limit of the fit:
*/
int
CSpectrumFit::low() const
{
  return m_low;
}
/*!
  Select the high limit for the fitline:
*/
int
CSpectrumFit::high() const
{
  return m_high;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////// Private utilities ///////////////////////////////
///////////////////////////////////////////////////////////////////////////

/*
   Locate our spectrum.. returns a pointer to it or NULL if not found.
*/
CSpectrum*
CSpectrumFit::findSpectrum()
{
  SpecTcl* papi = SpecTcl::getInstance();
  return   papi->FindSpectrum(getName());
}
