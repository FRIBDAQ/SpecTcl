#ifndef __CFILTEROUTPUTSTAGECREATOR_H
#define __CFILTEROUTPUTSTAGECREATOR_H

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

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif


class CFilterOutputStage;

/*! 
   Abstract base class for entities that can create filter output stages for
   the CFilterOutputStageFactory singleton.   These are part of the extensible
   factory pattern for filter output formatting.

   A concrete class must be created for each output format you want filter files
   to be written in.

   See CFilterOutputStageFactory.cpp for the registration of the built in formatters.
   I anticipate that many formatters will be written as plugins so that they can be
   built separately from SpecTcl only in the event a specific foreign analysis 
   product (e.g. CERN Root) is available.

   The comments prior to each virtual function in the class describe the responsibilities
   of the concrete class for the implementation of that function, as this class
   is pure abstract.

*/
class CFilterOutputStageCreator
{
public:
  /*!  
    Destruction is the only canonical I need to provide and implement.
    It is necessary to ensure that the destructor is appropriately polymorphic
    and chains through the hierarchy>
  */
  ~CFilterOutputStageCreator() {}


  /*!
     This function is called by the factory as it searches for a creator
     for a specific output stage type.  Your responsibility is to 
     compare the type parameter and, if it matches the type of 
     output stage you are willing to produce, return a newly allocated
     output stage.

     \param type - the type of output stage being requested by the factor.
     \return CFilterOutputStage*
     \retval NULL - This creator does not produce the requested type of output
                    stage.
     \retval other - A pointer to a newly created output stage for this type.

  */
  virtual CFilterOutputStage*  operator()(std::string type) = 0;

  /*!
      This function is called by the factory to document the set of output stage types
      that are available.  This should be a short string of the form:
      \verbatim
                      1         2         3
      col:  0123456789012345678901234567890
            type      - description

      \endverbatim

      \return std::string
      \retval see above.
  */
  virtual std::string document() const = 0;

  /*!
     In order to allow the factory to ensure that it is the one managing
     the storage of its creators (in case it is destroyed), all creators
     must implement this virtual copy constructor  to clone a copy of themselves
     in dynamic storage.  Usually this is best implemented by implementing a copy 
     construtor, and then having this function look like:
     \verbatim

     return *this;

     \endverbatim

  */

  virtual CFilterOutputStageCreator* clone() = 0;
};


#endif
