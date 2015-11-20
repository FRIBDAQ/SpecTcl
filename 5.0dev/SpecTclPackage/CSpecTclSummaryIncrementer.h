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

#ifndef _CSPECTCLSUMMARYINCREMENTER_H
#define _CSPECTCLSUMMARYINCREMENTER_H

/**
 * @file CSpecTclSummaryIncrementer.h
 * @brief Define the incrementer for SpecTcl summary spectra.
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

/**
 * @class CSpecTclSummaryIncrementer
 *
 * 
 *  This class implements the incrementer for a summary spectrum.
 * Summary spectra are basically a bunch of 1-d spectra displayed
 * in color intensity stacked left to right.  They allow one to quickly
 * pick out problema in an array of similar detectors by the break in the
 * pattern such problems usually cause.
 */
class CSpecTclSummaryIncrementer : public CSpecTcl2dBaseIncrementer 
{
private:
  std::vector<std::string>                          m_Names;
  std::vector<CParameterDictionary::pParameterInfo> m_Params;
  bool                                              m_bound;

  // Canonicals
public:
  CSpecTclSummaryIncrementer(std::vector<std::string> paramNames);
  virtual ~CSpecTclSummaryIncrementer();

  // Implementation of base class elements.

public:
  virtual void validate(CSpectrum& spec);
  virtual void operator()(CSpectrum& spec);

 private:
  virtual void validate();

};
#endif
