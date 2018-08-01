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

#ifndef CGAUSSIANFITCREATOR_H
#define CGAUSSIANFITCREATOR_H

#include "CFitCreator.h"

/*!
   This class creates guassian fit objects for the fit factory.
   the fit factory is an extensible factory that generates fits for the
   SpecTcl fitting framework.  It relies  on a set of registered
   fit creators to know how to create fit objects on demand.
   The Gaussian fit creator is one of the 'standard' fit objects.
   It is also possible to register additional user fit types.
*/
class CGaussianFitCreator : public CFitCreator
{
  // Canonicals:
public:
  CGaussianFitCreator();
  virtual ~CGaussianFitCreator();
  CGaussianFitCreator(const CGaussianFitCreator& rhs);
  CGaussianFitCreator& operator=(const CGaussianFitCreator& rhs);
  int operator==(const CGaussianFitCreator& rhs) const;
  int operator!=(const CGaussianFitCreator& rhs) const;

  // Virtual function implementations/overrides

  virtual CFit* operator()(std::string name, int id=0);
  virtual std::string DescribeFit();
};


#endif
