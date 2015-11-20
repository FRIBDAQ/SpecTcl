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

#include "CHistogrammerFactory.h"

/**
 * @file CHistogrammerFactory.cpp
 * @brief provide the base class behavior for the histogrammer factory (mostly throw).
 */

/**
 * create2dAllocator
 *
 *  Base class behavior assumes that often the 1d and 2d allocators
 *  can be the same...compute the storage requirements and allocate storage.
 *  this code just calls create1dAllocator.
 *
 * @return CSpectrumAllocator* pointer to the generated allocator.
 */
CSpectrumAllocator*
CHistogrammerFactory::create2dAllocator()
{
  return create1dAllocator();
}
/**
 * create1dIncrementer
 *
 *   Create a spectrum incrementer suitable for a simple 1d spectrum.
 *   Base class behavior is to throw an exception indicating this
 *   kernel does not support this type of spectrum.
 *
 * @param xParams - vector of x parameter names.
 * @param yParams - vector of y parameter names.
 */
CSpectrumIncrementer*
CHistogrammerFactory:: create1dIncrementer(std::vector<std::string> xParams, 
					   std::vector<std::string> yParams) 
{
  throwUnimplemented("1-d");
  return 0;			// in case the compiler wants to see a return val.
}

/**
 * create2dIncrementer
 *
 *   Create a spectrum incrementer suitable for a simple 2d spectrum.
 *   Base class behavior is to throw an exception indicating this
 *   kernel does not support this type of spectrum.
 *
 * @param xParams - vector of x parameter names.
 * @param yParams - vector of y parameter names.
 */
CSpectrumIncrementer*
CHistogrammerFactory::create2dIncrementer(std::vector<std::string> xParams, 
					  std::vector<std::string> yParams)
{
  throwUnimplemented("2-d");
  return 0;			// in case the compiler wants to see a return val.
}
/**
 * createSum1dIncrementer
 *
 *  Create a 1d spectrum that is a simple multiply incremented spectrum.
 *  This spectrum type was called a gamma spectrum and could have a fold
 *  applied to it.
 *  Default action - throw.
  *
 * @param xParams - vector of x parameter names.
 * @param yParams - vector of y parameter names.
*/
CSpectrumIncrementer*
CHistogrammerFactory::createSum1dIncrementer(std::vector<std::string> xParams, 
					     std::vector<std::string> yParams)
{
  throwUnimplemented("1d-sum");
  return 0;			// in case the compiler wants to see a return val.
}
/**
 * create2dComboIncrementer
 *
 *  Create a 2d spectrum that incrments for each pair of parameters.
 *   This was once a g2d spectrum and can have a fold applied to it.
 *
 *  Default action - throw.
 */
CSpectrumIncrementer*
CHistogrammerFactory::create2dComboIncrementer(std::vector<std::string> xParams, 
					       std::vector<std::string> yParams)
{
  throwUnimplemented("2d-combo");
  return 0;			// in case the compiler wants to see a return val.

}
/**
 * create2dSummaryIncrementer
 *
 *   Create an incrementer that can do a 2d summary spectrum.
 *
 *  default action throw:
 *
 * @param xParams - vector of x parameter names.
 * @param yParams - vector of y parameter names.
 */
CSpectrumIncrementer*
CHistogrammerFactory::create2dSummaryIncrementer(std::vector<std::string> xParams, 
						 std::vector<std::string> yParams)
{
  throwUnimplemented("2d-summary");
  return 0;
}
/**
 * create2dSumIncrementer
 *
 *  Create an incrementer that just sums multiple 2ds into a single
 *  2-d.
 *
 *
 * @param xParams - vector of x parameter names.
 * @param yParams - vector of y parameter names.
 */
CSpectrumIncrementer*
CHistogrammerFactory::create2dSumIncrementer(std::vector<std::string> xParams, 
					     std::vector<std::string> yParams)
{
  throwUnimplemented("2d-sum");
  return 0;
}
/**
 * createGammaDeluxeIncrementer
 *
 *  Create an incrementer for a 2d gamma deluxe spectrum.
 *
 *  Default action is to throw.
 *
 * @param xParams - vector of x parameter names.
 * @param yParams - vector of y parameter names.
 */
CSpectrumIncrementer*
CHistogrammerFactory::createGammaDeluxeIncrementer(std::vector<std::string> xParams, 
						   std::vector<std::string> yParams)
{
  throwUnimplemented("gamma-deluxe");
  return 0;
} 
/**
 * createGammaSummaryIncrementer
 * 
 *  Create a summary spectrum where each column is a gamma spectrum.
 *
 * Default action is to throw.
 *
 * @param xParams - vector of x parameter names.
 * @param yParams - vector of y parameter names.
 */
CSpectrumIncrementer*
CHistogrammerFactory::createGammaSummaryIncrementer(std::vector<std::string> xParams, 
						    std::vector<std::string> yParams)
{
  throwUnimplemented("gamma-sumary");
  return 0;
}
/*
 *----------------------------------------------------------------
 * Protected utilities
 */

/**
 * throwUnimplemented
 *   Throws an exception indicating that a histogramming kernel does
 * not support a specific spectrum type.
 *
 * @param methodName - the spectrum type that can't be created.
 */
void
CHistogrammerFactory::throwUnimplemented(const char* methodName) 
{
  std::string msg = "This histogrammer factory does not support creating ";
  msg += methodName;
  msg += " spectra.";
  throw histogrammer_factory_exception(msg.c_str());
}
