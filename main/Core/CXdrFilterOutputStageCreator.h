#ifndef __CXDRFILTEROUTPUTSTAGECREATOR_H
#define __CXDRFILTEROUTPUTSTAGECREATOR_H
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


#ifndef __CFILTEROUTPUTSTAGECREATOR_H
#include "CFilterOutputStageCreator.h"
#endif


/*!
   Output stage creator for the NSCL Xdr filter file output format.
   This creator recognizes the keyword "xdr" and creates a 
   CXdrFilterOutputStage.

*/
class CXdrFilterOutputStageCreator : public CFilterOutputStageCreator
{
public:
  // Canonicals. We only require copy construction for clone:
  // bit by bit operations are just fine for comparsions etc.

  CXdrFilterOutputStageCreator();
  CXdrFilterOutputStageCreator(const CXdrFilterOutputStageCreator& rhs);
  virtual ~CXdrFilterOutputStageCreator();

  // Implement the CFilterOutputStage Creator interface:

public:

  virtual CFilterOutputStage*  operator()(std::string type);
  virtual std::string document() const;
  virtual CFilterOutputStageCreator* clone();
};



#endif
