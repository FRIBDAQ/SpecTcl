/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2013.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef _CSPECTCLHISTOGRAMMERFACTORY_H
#define _CSPECTCLHISTOGRAMMERFACTORY_H

/**
 * @file CSpecTclHistogrammerFactory.h
 * @brief Defines the histogrammer factory specialized for SpecTcl spectra.
 */

#ifndef _CHISTOGRAMMERFACTORY_H
#include "CHistogrammerFactory.h"
#endif


/**
 * @class CSpecTclHistogrammerFactory
 * 
 *   Seek CHistogrammerFactory for more information about what this is trying to
 *   accomplish.  This class is a histogrammer factory that is specialized
 *   for histograms that are just arrays of simple storage.  This is the kind of
 *   spectrum the old style SpecTcl used to produce.
 *
 */
class CSpecTclHistogrammerFactory : public CHistogrammerFactory
{
public:
  // Allocator factory methods:

  virtual CSpectrumAllocator* create1dAllocator();

  // incrementer factory methods:

  virtual CSpectrumIncrementer* create1dIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  virtual CSpectrumIncrementer* create2dIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  virtual CSpectrumIncrementer* createSum1dIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  virtual CSpectrumIncrementer* create2dComboIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  /*
  virtual CSpectrumIncrementer* create2dSummaryIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  virtual CSpectrumIncrementer* create2dSumIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  virtual CSpectrumIncrementer* createGammaDeluxeIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  virtual CSpectrumIncrementer* createGammaSummaryIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  */

  // Utility methods:

private:
  void checkExactParamCount(std::vector<std::string> params, size_t requiredCount, 
			    std::string method, std::string tail);
  void checkAtLeastParamCount(std::vector<std::string> params, size_t requiredCount, 
			    std::string method, std::string tail);
  void checkParameterExists(std::string pName, std::string method, std::string name);
};



#endif
