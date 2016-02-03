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

// Implementation of the CXdrFilterOutputStageCreator class.

#include <config.h>
#include "CXdrFilterOutputStageCreator.h"
#include <CXdrFilterOutputStage.h>


using namespace std;


/*!
   Default constructor.
*/
CXdrFilterOutputStageCreator::CXdrFilterOutputStageCreator()
{}

/*!
  Copy constructor
*/
CXdrFilterOutputStageCreator::CXdrFilterOutputStageCreator(const CXdrFilterOutputStageCreator& rhs)
{}

/*!
  Destructor:
*/
CXdrFilterOutputStageCreator::~CXdrFilterOutputStageCreator()
{}

/*!
   If necessary create a new filter output stage.   This will happen if we are
   asked to produce an output stage of type "xdr".

   \param type  - The type of output stage desired.
   \return CFilterOutputStage*
   \retval NULL - if type != "xdr".
   \retval other- if type == "xdr".

*/
CFilterOutputStage*
CXdrFilterOutputStageCreator::operator()(string type)
{
  if (type == "xdr") {
    return new CXdrFilterOutputStage;
  } 
  else {
    return static_cast<CFilterOutputStage*>(NULL);
  }
}
/*!
   Document the type of output stage we create.  In this case
   we will emit a string like:
   "xdr        - NSCL XDR system independent filter file format"

   \return std::string
   \retval (see above).

*/
string
CXdrFilterOutputStageCreator::document() const
{
  return string("xdr        - NSCL XDR system independent filter file format");
}
/*!
    Clone.. this is essentially a virtual copy constructor.
    \return CFilterOutputStage*
    \retval Pointer to a dynamically allocated copy of this formatter.

*/
CFilterOutputStageCreator*
CXdrFilterOutputStageCreator::clone()
{
  return new CXdrFilterOutputStageCreator(*this);
}

