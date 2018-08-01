/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


#ifndef XAMINESPECTRUMINTERFACE_H
#define XAMINESPECTRUMINTERFACE_H

#include <SpectrumQueryInterface.h>
#include "dispshare.h"

class XamineSpectrumQuerier : public Win::SpectrumQuerier
{

  public:
    virtual Win::SpectrumQueryResults getSpectrumInfo(int slot);
    virtual Win::SpectrumQueryResults getSpectrumInfo(const std::string& name);


  private:
    int computeDimension(spec_type type);
};

#endif
