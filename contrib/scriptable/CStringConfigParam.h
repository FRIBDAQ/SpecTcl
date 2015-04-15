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

//! \class: CStringConfigParam           
//! \file:  .h
// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef __CSTRINGCONFIGPARAM_H  //Required for current class
#define __CSTRINGCONFIGPARAM_H

//
// Include files:
//

                               //Required for base classes
#ifndef __CCONFIGURATIONPARAMETER_H     //CConfigurationParameter
#include "CConfigurationParameter.h"
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif
 
// Forward class defintions.

class CTCLInterpreter;
class CTCLResult;
 
/*!
Represents  a string valued configuration parameter.

*/
class CStringConfigParam  : public CConfigurationParameter        
{

public:
	// Constructors and other cannnonical member functions.
	
    CStringConfigParam (const std::string& keyword);
    virtual  ~CStringConfigParam ( );  //Destructor - Delete any pointer data members that used new in constructors 
  
  CStringConfigParam (const CStringConfigParam& aCStringConfigParam );
  CStringConfigParam& operator= (const CStringConfigParam& rhs);
  int operator== (const CStringConfigParam& rhs) const;
  int operator!= (const CStringConfigParam& rhs) const {
	return !(operator==(rhs));
  }

  // Class functions:
 public:
    std::string getOptionValue ()   ; // 
    virtual   int SetValue (CTCLInterpreter& rInterp, CTCLResult& rResult, 
				    const char* value)   ; // 
    virtual std::string GetParameterFormat();

};

#endif
