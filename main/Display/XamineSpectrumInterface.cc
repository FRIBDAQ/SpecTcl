

#include "XamineSpectrumInterface.h"
#include "dispshare.h"
#include "xamineDataTypes.h"

extern volatile  spec_shared *xamine_shared;

Win::SpectrumQueryResults
XamineSpectrumQuerier::getSpectrumInfo(int slot)
{
  Win::SpectrumQueryResults results = {false, 0, 0, 0, std::string()};
  if (xamine_shared) {
    spec_type type = xamine_shared->gettype(slot);
    if (type != undefined) {
      results.s_exists = true;
      results.s_dimension = computeDimension(type);
      results.s_nBinsX = xamine_shared->getxdim(slot);
      if (results.s_dimension == 2) {
        results.s_nBinsY = xamine_shared->getydim(slot);
      }
      spec_title name;
      xamine_shared->getname(name, slot);
      results.s_name = std::string(name, name+sizeof(spec_title));
    }
  } else {
    // lack of xamine_shared means no spectra exist.
  }

  return results;
}

Win::SpectrumQueryResults
XamineSpectrumQuerier::getSpectrumInfo(const std::string& name)
{
  int slot = Xamine_GetSpectrumId(const_cast<char*>(name.c_str()));
  return getSpectrumInfo(slot);
}

int XamineSpectrumQuerier::computeDimension(spec_type type) 
{
  int dimension = 0;
  switch (type) {
    case twodlong:
    case twodword:
    case twodbyte:
      dimension = 2;
      break;
    case onedlong:
    case onedword:
      dimension = 1;
      break;
  }
  return dimension;
}
