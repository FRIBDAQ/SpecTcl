
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
