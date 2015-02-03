/*
   Implementation of the CCalibrationDistribtuion class.
   See CalibrationDistribution.h for more details.
*/
#include <config.h>
#include "CalibrationDistribution.h"
#include "GaussianDistribution.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
    Constructor Create a new calibration distribution. A calibration distribution
    has the following parameters that are passed in as arguments at construction time:
    \param fFirstPosition (Float_t):
        The centroid of the first peak. 
    \param nPeakCount (UInt_t):
        The number of distinct peaks to produce into the parameter.
    \param fPeakInterval (Float_t):
        The fixed interval between adjacent peaks.
    \param fPeakWidth (Float_t):
        The width of each of the peaks.  All peaks will have the same width.
    \param fMaximumValue (Float_t):  The cutoff value of the random number.

*/
CCalibrationDistribution::CCalibrationDistribution(Float_t fFirstPosition,
						   UInt_t  nPeakCount,
						   Float_t fPeakInterval,
						   Float_t fPeakWidth,
						   Float_t fMaximumValue) :
  m_Distribution(*(new CGaussianDistribution(fFirstPosition, fPeakWidth, 
					     fMaximumValue))),
  m_fInterval(fPeakInterval),
  m_nPeaks(nPeakCount),
  m_nSample(0)
  {
}
/*!  Copy constructor.  This function produces a functional duplicate of the
  rhs parameter.  Note that since we're storing the underlying distribution
  as a reference, we need to play a bit of fancy footwork to use DoAssign.
  We need to create a dummy distribution that will then have the real one
  copied in to it.
  \param rhs (const CCalibrationDistribution& [in]):
      The distribution being copied.

*/
CCalibrationDistribution::CCalibrationDistribution(const CCalibrationDistribution& rhs) :
  m_Distribution(*(new CGaussianDistribution(0.0, 0.0, 0.0)))	// This gets overridden...

{
  DoAssign(rhs);
}
/*!
   Assignment operator.  This replaces the current fully constructed object
   with the contents of the rhs parameter... creating a copy into this object.
   \param rhs (const CCalibrationDistribution& [in]):
       The source for the copy.
   \return  CCalibrationDistribution &
   \retval   *this
*/
CCalibrationDistribution&
CCalibrationDistribution::operator=(const CCalibrationDistribution& rhs)
{
  if(this != &rhs) {
    CDistribution::operator=(rhs);
    DoAssign(rhs);
  }
  return *this;
}

/*!
   Destructor for the class
*/
CCalibrationDistribution::~CCalibrationDistribution()
{
  delete &m_Distribution;
}

/*!
   Equality comparison.  The definition of equality for this distribution is that
   \em all of the following must be satisfied:
   - The base class indicates equality.
   - The two distributions are equal.
   - All members except the sample counter are equal.

   \param rhs (const CCalibrationDistribution& [in]):
       The object to compare to this.
   \return int
   \retval  0   - unequal
   \retval  1   - equal
*/
int
CCalibrationDistribution::operator==(const CCalibrationDistribution& rhs)
{
  return (CDistribution::operator==(rhs)                              &&
	  (m_Distribution == rhs.m_Distribution)                      &&
	  (m_fInterval    == rhs.m_fInterval)                         &&
	  (m_nPeaks       == rhs.m_nPeaks));
}
/*!
   Inequality comparison.  The definition of inequality is the logical
   negation of equality.

   \param rhs (const CCalibrationDistribution& [in]):
       The object to compare to this.
   \return int
   \retval  0   - equal
   \retval  1   - inequal
*/
int
CCalibrationDistribution::operator!=(const CCalibrationDistribution& rhs)
{
  return !(operator==(rhs));
}


// Class operations:

/*!
   Sample from the calibration distribution.  The sample is
   taken and an offset (m_nSample*m_fInterval) is added to it, then
   the sample is trimmed to the cutoff.  The m_nSample is then circularly 
   incremented to set up for the next time.
   \return UInt_t
   \retval A sample from one of the peaks.
*/
Float_t
CCalibrationDistribution::operator()()
{

  // Create the raw sampled value....

  UInt_t nValue = (UInt_t)m_Distribution();                 // Sample the distribution...
  nValue       += (UInt_t)((Float_t)m_nSample * m_fInterval); // Shift to the correct peak.


  // Cyclically increment the sample no.

  m_nSample++;
  m_nSample = m_nSample % m_nPeaks;

  // Limit the raw sampled value to the endpoint and return it:

  if(nValue > m_Distribution.getScale()) {
    nValue = (UInt_t)m_Distribution.getScale();
  }
  return (Float_t)nValue;
}
/*
   Utility function to assign to *this from rhs.
*/
void
CCalibrationDistribution::DoAssign(const CCalibrationDistribution& rhs)
{
  m_Distribution = rhs.m_Distribution;
  m_fInterval    = rhs.m_fInterval;
  m_nPeaks       = rhs.m_nPeaks;
  m_nSample      = rhs.m_nSample;


}
