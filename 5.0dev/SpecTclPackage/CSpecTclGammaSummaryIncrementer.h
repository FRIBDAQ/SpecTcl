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
#ifndef _CSPECTCLGAMMASUMMARYINCREMENTER_H
#define _CSPECTCLGAMMASUMMARYINCREMENTER_H


/**
 * @file CSpecTclGammaSumaryIncrementer.h
 * @brief Definition of an incrementer for Gamma summary spectra
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
 * @class CSpecTclGammaSummaryIncrementer
 *
 *  A SpecTcl Gamma summary spectrum is a spectrum each of whose
 *  X axis channels has a gamma spectrum on the Y axis.
 *  This is an exact analogy with the summary spectrum.
 *
 *  This spectrum has the unique feature that it can have a partial binding:
 *  The binding is a per channel thing where each channel that is bound will increment
 *  but those that are not bound won't.
 *
 */
class CSpecTclGammaSummaryIncrementer : public CSpecTcl2dBaseIncrementer 
{
  // Public data types:
public:
  typedef std::vector<std::string>         ChannelParameterNames;
  typedef std::vector<ChannelParameterNames>   SpectrumParameterNames;

  typedef std::vector<CParameterDictionary::pParameterInfo> ChannelParameters;
  typedef std::vector<ChannelParameters>                   SpectrumParameters;

  // internal data

  SpectrumParameterNames    m_names;
  SpectrumParameters        m_params;
  std::vector<bool>         m_bound;

  // Canonicals:

public:
  CSpecTclGammaSummaryIncrementer(SpectrumParameterNames names);
  virtual ~CSpecTclGammaSummaryIncrementer() {}

  // interface methods.

public:

  virtual void validate(CSpectrum& spec);
  virtual void operator()(CSpectrum& spec);

private:
  void validate();


};


#endif
