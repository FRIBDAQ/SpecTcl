/*!
    Calibration distribution is a class that produces evenly space peaks.
    for simplicitly all peaks have the same width.  Peaks are iterated.
    This is really done by having a single distribution and adding
    a cyclical offset to the sampled value on each sample.
*/
#ifndef CALIBRATIONDISTRIBUTION_H
#define CALIBRATIONDISTRIBUTION_H

#include <histotypes.h>
#include <Distribution.h>

class  CGaussianDistribution;

class CCalibrationDistribution : public CDistribution {
  // Private member data:
private:
  CGaussianDistribution&  m_Distribution;
  float                   m_fInterval;
  int                     m_nPeaks;
  int                     m_nSample;

  // Constructors and other cannonicals:
public:
  CCalibrationDistribution(Float_t fFirstPosition, 
			   UInt_t  nPeakCount,
			   Float_t fPeakInterval,
			   Float_t fPeakWidth,
			   Float_t fMaximumValue);
  CCalibrationDistribution(const CCalibrationDistribution& rhs);
  ~CCalibrationDistribution();

  CCalibrationDistribution& 
    operator=(const CCalibrationDistribution& rhs);
  int operator==(const CCalibrationDistribution& rhs);
  int operator!=(const CCalibrationDistribution& rhs);

  // Operations on the class...

public:
  virtual Float_t operator()();

protected:
  void DoAssign(const CCalibrationDistribution& rhs);
  
};



#endif
