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

#ifndef _CSPECTCLCOMBO2DINCREMENTER_H
#define _CSPECTCLCOMBO2DINCREMENTER_H

/**
 * @file CSpecTclCombo2dIncrementer.h
 * @brief Definition of a SpecTcl 2d incrementer for gamma combos.
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

#ifndef _STD_SET
#include <set>
#ifndef _STD_SET
#define _STD_SET
#endif
#endif

class CSpectrum;

/**
 * @class CSpecTclCombo2dIncrementer
 *
 *  A 2d combo spectrum is a type of gamma spectrum where
 *  each axis has an independent set of parameters.
 *  increments take place for all pairs of parameters
 *  that are valid for the event.  Note that folds can cut down
 *  the set of parameters on each axis that are
 *  used for the increment.
 *
 * @todo Implement the infrastructure for folding spectra.
 *
 */
class CSpecTclCombo2dIncrementer : public CSpecTcl2dBaseIncrementer
{
  // Local data:
private:
  std::vector<std::string>                       m_xNames;
  std::vector<std::string>                       m_yNames;

  std::set<CParameterDictionary::pParameterInfo> m_xParams;
  std::set<CParameterDictionary::pParameterInfo> m_yParams;

  bool                                           m_fullyBound;

  // canonicals

public:
  CSpecTclCombo2dIncrementer(std::vector<std::string> xParams, 
			     std::vector<std::string> yParams);
  virtual ~CSpecTclCombo2dIncrementer();

  // The incrementer interface

public:
  virtual void validate(CSpectrum& spec);
  virtual void operator()(CSpectrum& spec);

  // Utilties
private:
  std::set<CParameterDictionary::pParameterInfo> validate(std::vector<std::string> names);
  void validate();
  void fold() {};		/* Stub. */

};

#endif
