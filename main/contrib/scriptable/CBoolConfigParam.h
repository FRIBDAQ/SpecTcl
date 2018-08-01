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


// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//

#ifndef CBOOLCONFIGPARAM_H  //Required for current class
#define CBOOLCONFIGPARAM_H

//
// Include files:
//

                               //Required for base classes
#include "CConfigurationParameter.h"
#include <histotypes.h>
 
/*!
Encapsulates a boolean parameter. Bool parameters
are keyword value pairs.  The value is any string that
is parseable by Tcl_ExprBoolean.  True values are:
- 1
- true
- yes
- on
False values are:
- 0
- false
- no
- off

An example of a parameter of this sort might be:
\verbatim
-eclreadout off
\endverbatim
Turn s off ecl readout for e.g. a fera.
*/
class CBoolConfigParam  : public CConfigurationParameter        
{
private:
  bool m_fValue;            //!< Current boolean value. : 
  //  Constructors and other cannonical member functions:
public:
  CBoolConfigParam (const std::string& rName, 
                    bool fDefault = false); //!< Constructor
 ~ CBoolConfigParam ( );                    //!< Destructor
  CBoolConfigParam (const CBoolConfigParam& rhs); //!< copy constructor
  CBoolConfigParam& operator= (const CBoolConfigParam& rhs); //!< Assignment
  int operator== (const CBoolConfigParam& rhs) const; //!< Equality test
  int operator!= (const CBoolConfigParam& rhs) const
  {
    return !operator==(rhs);
  }

  // Selectors:

public:

  bool getValue() const {
    return m_fValue;
  }
  // Mutators
protected:
  void setBool(bool fValue) {
      m_fValue = fValue;
  }
  // Class operations:

public:

  bool getOptionValue ()   ; //!< get current flag. 
  virtual   int SetValue (CTCLInterpreter& rInterp, 
                          CTCLResult& rResult, 
                          const char* pFlag)   ; //!< Parse flag.
  virtual std::string GetParameterFormat();
 protected:
  static Bool_t ParseFlag(const char* value);
};

#endif
