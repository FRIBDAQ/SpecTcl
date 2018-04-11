//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins 
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321



#include "XamineSpectrumInterface.h"
#include "dispshare.h"
#include "xamineDataTypes.h"

extern volatile  spec_shared *xamine_shared;

Win::SpectrumQueryResults
XamineSpectrumQuerier::getSpectrumInfo(int slot)
{
  Win::SpectrumQueryResults results = {false, 0, 0, 0, std::string(), slot};
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
  int slot = Xamine_GetSpectrumId(name);
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
