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
#include "CSpecTclHistogrammerFactory.h"


/**
 * @file CSpecTclHistogrammerFactory.cpp
 * @brief Implementation of the SpecTcl type histogrammer factory.
 */


#include "CSpecTclSpectrumAllocator.h"
#include "CParameterDictionary.h"
#include "CSpecTcl1dIncrementer.h"
#include "CSpecTcl2dIncrementer.h"
#include "CSpecTclSum1dIncrementer.h"

#include <sstream>

/**
 * create1dAllocator 
 *
 * @return CSpectrumAllocator*  - Pointer to what is actually a CSpecTclSpectrumAllocator
 */
CSpectrumAllocator* 
CSpecTclHistogrammerFactory::create1dAllocator() 
{
  return new CSpecTclSpectrumAllocator;
}


/**
 * create1dIncrementer
 *
 *   Creates an incrementer for a normal 1d spectrum.
 *
 * @param xParmas - One element array containing the x parameter name to use.
 * @param yParams - 0 element array, unused.
 *
 * @return CSpectrumIncrementer* a dynamically allocated spectrum incrementer
 * @throw histogrammer_factory_exception if:
 *         - There is not exactly 1 x parameter.
 *         - There is not exactly 0 y parameters.
 *         - The x parameter is nonexistent.
 *
 */
CSpectrumIncrementer*
CSpecTclHistogrammerFactory::create1dIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams)
{
  // Check that we have the right number of x/y params:

  checkExactParamCount(xParams, 1, "CSpecTclHistogrammerFactory::create1dIncrementer", "X parameter");
  checkExactParamCount(yParams, 0, "CSpecTclHistogrammerFactory::create1dIncrementer", "Y parameters");

  // Check that the X parameter is defined:

  checkParameterExists(xParams[0], "CSpecTclHistogrammerFactory::create1dIncrementer", "X parameter");

  return new CSpecTcl1dIncrementer(xParams[0]);
}
/**
 * create2dIncrementer
 *    Creates an incrementer for a standard 2d spectrum (an x and y parameter).
 *
 *  @param xParams - Should be a 1 element array containing the X parameter.
 *  @param yparams - Should be a 1 element array containing the Y parameter.
 *
 *  @return CSpectrumIncrementer* - A dynamically allocated spectrum incrementer.
 *  @throw histogrammer_factory_exception if:
 *     -  There is not exactly one X parameter.
 *     -  There is not exactly 1 Y parameter.
 *     -  Either the x or y parameter are not defined.
 */
CSpectrumIncrementer*
CSpecTclHistogrammerFactory::create2dIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams)
{
  // Check that we have the right number of x/y params:

  checkExactParamCount(xParams, 1, "CSpecTclHistogrammerFactory::create2dIncrementer", "X parameter");
  checkExactParamCount(yParams, 1, "CSpecTclHistogrammerFactory::create2dIncrementer", "Y parameter");


  // Validate existence of the parameters:

  checkParameterExists(xParams[0], "CSpecTclHistogrammerFactory::create2dIncrementer", "X parameter");
  checkParameterExists(yParams[0], "CSpecTclHistogrammerFactory::create2dIncrementer", "Y parameter");

  return new CSpecTcl2dIncrementer(xParams[0], yParams[0]);
}
/**
 *  createSum1dIncrementer
 *
 *  Create a summary spectrum incrementer.  This takes an array of x parameters 
 *  and no Y parameters.  The spectrum managed basically is a bunch of 1-d
 * spectra where the y axis for each channel is the spectrum for the corresponding
 * parameter, e.g. 0,y is the y'th channel of the spectrum xParams[0]...
 *
 * @param xParams - vector of at least one x parameter (really boring if only 1).
 * @param yParams - empty vector unused.
 *
 * @return CSpectrumIncrementer* - Pointer to a dynamically allocated spectrum incrementer.
 * @throw histogrammer_factory_exception if:
 *       -  There are no X parmaeters.
 *       -  There are Y parameters.
 *       -  Any X parameter does not exist.
 */
CSpectrumIncrementer*
CSpecTclHistogrammerFactory::createSum1dIncrementer(std::vector<std::string> xParams, 
						    std::vector<std::string> yParams)
{
  // Need at least 1 x parameter and 0 y:

  checkAtLeastParamCount(xParams, 1, "CSpecTclHistogrammerFactory::createSum1dIncrementer", "X parameter");
  checkExactParamCount(yParams, 0,
		       "CSpecTclHistogrammerFactory::createSum1dIncrementer", "Y parameters");

  // The x parameters must exist.

  for (int i =0; i < xParams.size(); i++) {
    checkParameterExists(xParams[i], "CSpecTclHistogrammerFactory::createSum1dIncrementer", xParams[i]);
  }
  // Everything's going to work now:

  return new CSpecTclSum1dIncrementer(xParams);
}
/**
 * create2dComboIterator
 *
 *   Creates an incrementer for what used to be called a 'gamma deluxe' spectrum.
 *   This is a spectrum with independent x/y parameter sets that is incremented
 *   for each valid pair of parameters.
 *
 * @param xParams - vector of names of parameters on the x axis.
 * @param yParams - vector of names of parameters on the y axis.
 *
 * @return CSpectrumIncrementer* - pointer to a dynamically allocatd spectrum incrementer.
 * @throw histogrammer_factory_exception if:
 *     - There are no X parameters.
 *     - There are no Y parameters.
 *     - Any of the X parameters is not yet defined.
 *     - Any of the Y parameters is not yet defined.
 */
CSpectrumIncrementer*
CSpecTclHistogrammerFactory::create2dComboIncrementer(std::vector<std::string> xParams, 
						      std::vector<std::string> yParams)
{
  checkAtLeastParamCount(xParams, 1, "CSpecTclHistogrammerFactory::create2dComboIncrementer", "X parameter");
  checkAtLeastParamCount(yParams, 1, "CSpecTclHistogrammerFactory::create2dComboIncrementer", "Y parameter");

  return 0;
}


/*--------------------------------------------------------------------------------
 * Private utility methods:
 */

/**
 * checkExactParamCount
 *
 *  Ensures that a vector of parameter names has exactly the required number
 *	  of elements:
 *
 * @param params        - Vector of parameter names.
 * @param requiredCount - Number of parameter names that must be present.
 * @param method        - Name of the method that wants to know (see exceptions below).
 * @param tail          - Tail end of the exception string (see exceptions below).
 *
 * @throw histogrammer_factory_exception - if the condition is false.  The string that defines the error
 *                       is put together as follows:
 *                       @method@ needs exactly @requiredCount@ @tail@
 *                       Where @xxx@ means substitute the stringified version of the paramter named xxx
 */
void
CSpecTclHistogrammerFactory::checkExactParamCount(std::vector<std::string> params, size_t requiredCount,
						  std::string method, std::string tail)
{
  if(params.size() != requiredCount) {
    std::stringstream msg;
    msg << method << " needs exactly " << requiredCount << " " << tail;
    throw histogrammer_factory_exception(msg.str().c_str());
  }
}
/**
 * checkAtLeastParamCount
 *
 *   Ensures a vector of parameters has at least the requested number of elements.
 *
 * @param params        - Vector of parameter names.
 * @param requiredCount - Number of parameter names that must be present.
 * @param method        - Name of the method that wants to know (see exceptions below).
 * @param tail          - Tail end of the exception string (see exceptions below).
 *
 * @throw histogrammer_factory_exception - if the condition is false.  The string that defines the error
 *                       is put together as follows:
 *                       @method@ needs at least @requiredCount@ @tail@
 *                       Where @xxx@ means substitute the stringified version of the paramter named xxx
 */
void
CSpecTclHistogrammerFactory::checkAtLeastParamCount(std::vector<std::string> params, size_t requiredCount,
						    std::string method, std::string tail)
{
  if(params.size() < requiredCount) {
    std::stringstream msg;
    msg << method << " needs at least " << requiredCount << " " << tail;
    throw histogrammer_factory_exception(msg.str().c_str());
  }}



/**
 * checkParameterExists
 *    
 *  If a named parameter has not yet been defined, throws a histogram_factory_exception
 *  with a 'well formed' error message.  See the exceptions specification.
 *
 * @param pName  - The name of the parameter (p in this case is short for parameter not pointer
 * @param method - The method that wants to know, this is used in the exception string.
 * @param name   - String that's used to describe what we'd like to see in the name part of the 
 *                 exception string may or may not be the same as pName.
 *
 * @throw histogrammer_factor_exception - If the parameter does not exist.  The exception
 *                                     string is of the form:
 *                                     "@method@ @name@ must exist"
 *                                     where @xxx@ means subsitute the value of that parameter
 *                                     in the string.
 * @note A use case for the name parameter being differnet than the name of the parameter is when
 *       the caller wants to identify and axis rather than a parameter e.g.:
 *       "CSpecTclHistogrammerFactory::create2dIncrementer X parameter does not exist"
 *       where:
 *       - @method@ is 'CSpecTclHistogrammerFactory::create2dIncrementer'
 *       - @name@ is 'X parameter'
 */
void
CSpecTclHistogrammerFactory::checkParameterExists(std::string pName, std::string method, std::string name)
{
  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator pParam = pDict->find(pName);
  if (pParam == pDict->end()) {
    std::string msg(method);
    method += " ";
    method += name;
    method += " does not exist";
    throw histogrammer_factory_exception(method.c_str());
  }
  
    
}
