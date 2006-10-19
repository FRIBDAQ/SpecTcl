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

#include <config.h>
#include "CGaussianFitCreator.h"
#include "CGaussianFit.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// The canonicals for this class are trivial at this time and don't merit
// any comment:

CGaussianFitCreator::CGaussianFitCreator() :
  CFitCreator()
{}

CGaussianFitCreator::~CGaussianFitCreator()
{}

CGaussianFitCreator::CGaussianFitCreator(const CGaussianFitCreator& rhs) :
  CFitCreator(rhs)
{}

CGaussianFitCreator& 
CGaussianFitCreator::operator=(const CGaussianFitCreator& rhs) 
{
  if (this != &rhs) {
    CFitCreator::operator=(rhs);
  }
  return *this;
}

int
CGaussianFitCreator::operator==(const CGaussianFitCreator& rhs) const
{
  return CFitCreator::operator==(rhs);
}

int 
CGaussianFitCreator::operator!=(const CGaussianFitCreator& rhs) const
{
  return !(*this == rhs);
}

/*!
   Create a gaussian fitting object and return it as a pointer to a polymorpic
   generic fit.
*/
CFit*
CGaussianFitCreator::operator()(string name, int id)
{
  return new CGaussianFit(name, id);
}
/*!
  Describe what we do for the studio audience:
*/
string
CGaussianFitCreator::DescribeFit()
{
  return string("Gaussian fit with baseline");
}
