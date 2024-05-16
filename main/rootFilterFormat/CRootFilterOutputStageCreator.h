#ifndef CROOTFILTEROUTPUTSTAGECREATOR_H
#define CROOTFILTEROUTPUTSTAGECREATOR_h
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
#include <CFilterOutputStageCreator.h>


/*!
   Defines a creator that allows filters to write their output data in root format.
*/
class CRootFilterOutputStageCreator : public CFilterOutputStageCreator
{
public:
  CRootFilterOutputStageCreator() {}
  CRootFilterOutputStageCreator(const CRootFilterOutputStageCreator& rhs);

  // We have to meet the following interface obligations:

  virtual CFilterOutputStage*  operator()(std::string type);
  virtual std::string document() const;
  virtual CFilterOutputStageCreator* clone();
};


#endif
