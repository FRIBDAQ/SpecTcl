#ifndef __CXDRFILTEROUTPUTSTAGE_H
#define __CXDRFILTEROUTPUTSTAGE_H
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

class CXdrOutputStream;


/*!
    The xdr filter output stage writes filtered data to file in an Xdr (machine independent)
    format.  Files consist of the following types of records:

    Records that describe the parameters the each event could contain:

   \verbatim
      +----------------------+
      | "header"             |
      +----------------------+
      | NumParams            |
      +----------------------+
      | Name string(1)       |
      +----------------------+
      | ...                  |
      +----------------------+
      |Name string(NumParams)| 
      +----------------------+
   \endverbatim

   The first entry a string "header" indicates this is the file header which describes the
   data.
   NumParams is an integer that describes the number of parameter names that follow.
   Name string(i) are strings which contain the names of each parameter.

   Records containing events:


   \verbatim
      +-------------------------+
      |   "event"               |
      +-------------------------+
      |NumParams bits of bitmask|
      +-------------------------+
      |Param for lowest setbit  |
      +-------------------------+
      |        ...              |
      +-------------------------+
      |Param for highest setbit |
      +-------------------------+

   \endverbatim

   The first entry is the sting "event" which indicates this record contains an event.
   The "NumParams bits of bitmask" are an array of integers, such that the number of bits
   is at least NumParams from the "header" record.  The bitmasks indicate which parameters are
   present in each event.  Each bit corresponds to a single parameter.  The lowest bit of
   the first integer is set if the parameter described by Name string(1) is present,
   and so on.

   The paramters themselves are floats, and only the ones for which bits are set are 
   supplied.
*/

class CXdrFilterOutputStage : public CFilterOutputStage
{
  // private data:
private:
  CXdrOutputStream*    m_pOutputEventStream;
  std::vector<UInt_t>  m_vParameterIds;

public:
  CXdrFilterOutputStage();
  virtual ~CXdrFilterOutputStage();

private:
  CXdrFilterOutputStage(const CXdrFilterOutputStage& rhs);
  CXdrFilterOutputStage& operator=(const CXdrFilterOutputStage& rhs);
  int operator==(const CXdrFilterOutputStage& rhs) const;
  int operator!=(const CXdrFilterOutputStage& rhs) const;

public:


  // We must implement the output stage interface:

public:
  virtual void open(std::string filename) ;
  virtual void close() ;
  virtual void DescribeEvent(std::vector<std::string> parameterNames,
			     std::vector<UInt_t>      parameterIds);

  virtual void operator()(CEvent& event);
  virtual std::string type() const;


  // Utilities:
private:
  void setBit(unsigned* bits, unsigned offset);
};


#endif
