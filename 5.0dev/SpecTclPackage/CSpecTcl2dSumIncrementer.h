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

#ifndef _CSPECTCL2DSUMINCREMENTER_H
#define _CSPECTCL2DSUMINCREMENTER_H

/**
 * @file CSpecTcl2dSumIncrementer.h
 * @brief Defines the iterator for 2d sum spectra (SpecTcl).
 */
#ifndef _CSPECTCL2DBASEINCREMENTER_H
#include "CSpecTcl2dBaseIncrementer.h"
#endif


#ifndef _CPARAMETERDICTIONARY_H
#include "CParameterDictionary.h"
#endif

#ifndef _STD_STRING
#include <string>
#ifndef _STD_STRING
#define _STD_STRING
#endif
#endif

#ifndef _STD_VECTOR
#include <vector>
#ifndef _STD_VECTOR
#define _STD_VECTOR
#endif
#endif


class CSpectrum;

/**
* @class CSpecTcl2dSumIncrementer
*
*   This class handles incrementing/accessing a 2d sum spectrum.
*   2d sum spectra take two ordered pairs of parameters and
*   produce the spectrum that would be the sum of all the 2d spectra
*   of corresponding parameters.
*   The spectrum only will increment if all parameters were defined
*   as of the last validate invocation.
*/
class CSpecTcl2dSumIncrementer : public CSpecTcl2dBaseIncrementer
{
private:
  std::vector<std::string> m_xNames;
  std::vector<std::string> m_yNames;

  std::vector<CParameterDictionary::pParameterInfo> m_xParams;
  std::vector<CParameterDictionary::pParameterInfo> m_yParams;

  bool m_bound;

  // canonicals
public:
  CSpecTcl2dSumIncrementer(std::vector<std::string> xNames, 
			   std::vector<std::string> yNames);
  virtual ~CSpecTcl2dSumIncrementer();

  // Implementations of the interface methods:

public:
  virtual void validate(CSpectrum& spec);
  virtual void operator()(CSpectrum& spec);

private:
  void validate();

};



#endif

 
