#ifndef __CROOTFILTEROUTPUTSTAGE_H
#define __CROOTFILTEROUTPUTSTAGE_H
 
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef __CFILTEROUTPUTSTAGE_H
#include "CFilterOutputStage.h"
#endif


class CRootNtupleAccumulator;

/*!
  Filter output stage that writes a root file with an n-tuple inside.
  The ntuple name will be "spectcl" until we can think of a way to give it a name.

  The parameter names in the ntpule will be the same as the names of the SpecTcl parameter names
  the filter should send to its output stage.

  In this implementation, any parameter that is not present will be given a value of NaN
  which hopefully will silently not histogram.

*/
class CRootFilterOutputStage : public CFilterOutputStage
{
private:
  CRootNtupleAccumulator& m_nTuple;
  std::vector<UInt_t>     m_parameterIds;

public:
  CRootFilterOutputStage();
  virtual ~CRootFilterOutputStage();
  
  // using references is nice but it makes copy like objects impossible.

private:
  CRootFilterOutputStage(const CRootFilterOutputStage& rhs);
  CRootFilterOutputStage& operator=(const CRootFilterOutputStage& rhs);
  int operator==(const CRootFilterOutputStage& rhs) const;
  int operator!=(const CRootFilterOutputStage& rhs) const;
public:

  // We must meet the following interface:

  virtual void open(std::string filename) ;
  virtual void close();
  virtual void DescribeEvent(std::vector<std::string> parameterNames,
			     std::vector<UInt_t>      parameterIds);
  virtual void operator()(CEvent& event) ;
  virtual std::string  type() const;
};


#endif
