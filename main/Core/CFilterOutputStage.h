#ifndef CFILTEROUTPUTSTAGE_H
#define CFILTEROUTPUTSTAGE_H
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

#include <string>
#include <vector>
#include <histotypes.h>


//

class CEvent;
class CEventFilter;


/*!  

  Abstract base class for expanding filters to support arbitrarily formatted output.
  for each event a filter passes, an object derived from this class will gain control
  to write it to an output file.
*/
class CFilterOutputStage
{
public:
  // Canonicals we need:
  
  virtual ~CFilterOutputStage(); // Usually needed for base classes.

  // The interface specfication:

public:
  virtual void open(std::string filename) = 0;
  virtual void close() = 0;
  virtual void onAttach(CEventFilter& filter);
  virtual void DescribeEvent(std::vector<std::string> parameterNames,
			     std::vector<UInt_t>      parameterIds) =0;
  virtual void operator()(CEvent& event) = 0;
  virtual std::string  type() const = 0;
};

#endif
