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

#ifndef _CHISTOGRAMMERFACTORY_H
#define _CHISTOGRAMMERFACTORY_H

#ifndef _STD_EXCEPTION
#include <exception>
#ifndef _STD_EXCEPTION
#define _STD_EXCEPTION
#endif
#endif

#ifndef _STD_VECTOR
#include <vector>
#ifndef _STD_VECTOR
#define _STD_VECTOR
#endif
#endif

#ifndef _STD_STRING
#include <string>
#ifndef _STD_STRING
#define _STD_STRING
#endif
#endif

#ifndef _STD_STRING
#include <string>
#ifndef _STD_STRING
#define _STD_STRING
#endif
#endif

/**
 * @file CHistogrammerFactory.h
 * @brief Defines the base class for a histogrammer factory.
 */

class CSpectrumAllocator;
class CSpectrumIncrementer;

/**
 * @class CHistogrammerFactory
 *
 *  This is a base class for a factory of the components of a spectrum.
 *  Two of the components of a spectrum consist of histogrammer
 *  kernel specific objects:
 *  - An allocator that is responsible for storage management and an
 *  - incrementer that is responsible for knowing how to modify the spectrum
 *    contents.
 *
 *  Each histogramming kernel is responsible for having a factory to produce
 *  these objects.  The alocators are relatively limited in number but the
 *  incrementers require one per spectrum type.  In the future this may
 *  be broken down into a kernel independent component that determines the
 *  increments for an event and a kerneld dependent piece that knows how to 
 *  apply those increments.  See CSpectrumIncrementer.h and CSpectrumIncrementer.cpp
 */
class CHistogrammerFactory 
{
public:
  // Allocator factory methods:

  virtual CSpectrumAllocator* create1dAllocator() = 0;
  virtual CSpectrumAllocator* create2dAllocator();

  // incrementer factory methods:

  virtual CSpectrumIncrementer* create1dIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  virtual CSpectrumIncrementer* create2dIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  virtual CSpectrumIncrementer* createSum1dIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  virtual CSpectrumIncrementer* create2dComboIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  virtual CSpectrumIncrementer* create2dSummaryIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  virtual CSpectrumIncrementer* create2dSumIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  virtual CSpectrumIncrementer* createGammaDeluxeIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
  virtual CSpectrumIncrementer* createGammaSummaryIncrementer(std::vector<std::string> xParams, std::vector<std::string> yParams);
protected:
  void throwUnimplemented(const char* methodName);
};

class histogrammer_factory_exception : public std::exception {
private:
  std::string m_message;
public:
  histogrammer_factory_exception(const char* msg) : m_message(msg) {}
  virtual ~histogrammer_factory_exception() throw() {}
  const char* what() const throw() {
    return m_message.c_str();
  }
  
};

#endif
