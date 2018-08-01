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
//
// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef CCONFIGURATIONPARAMETER_H  //Required for current class
#define CCONFIGURATIONPARAMETER_H

//
// Include files:
//

#include <string>


// Forward class defintions:

class CTCLInterpreter;
class CTCLResult;
  
/*!
This is the base class for all configuration parameter parsers.
Configuration parameter parsers accept pair of 
- keyword int
- keyword arrays of ints.
- keyword bool


*/
class CConfigurationParameter      
{
private:
  
  // Private Member data:
    std::string m_sSwitch;  //!  Switch keyword. e.g. -pedestal  
    std::string m_sValue;  //!   Value of parameter in std::string form.  
   
public:
  // Constructors and other canonical member functinos:
  CConfigurationParameter (const std::string& keyword);
  virtual  ~ CConfigurationParameter ( );  

    //Copy Constructor 

  CConfigurationParameter (const CConfigurationParameter& rhs);
  CConfigurationParameter& operator= (const CConfigurationParameter& rhs);
  int operator== (const CConfigurationParameter& rhs) const;
  int operator!= (const CConfigurationParameter& rhs) const {
    return !operator==(rhs);
  }
// Selectors:

public:

          //Get accessor function for non-static attribute data member
  std::string getSwitch() const
  {
    return m_sSwitch;
  }  
            //Get accessor function for non-static attribute data member
  std::string getValue() const
  {
    return m_sValue;
  }   

// Attribute mutators:

protected:

  void setSwitch (const std::string am_sSwitch)
  { 
    m_sSwitch = am_sSwitch;
  }  
  void setValue (const std::string am_sValue)
  {
    m_sValue = am_sValue;
  }   

  // Class operations:

public:

  bool Match (const std::string & rSwitch)   ; 
  virtual   int operator() (CTCLInterpreter& rInterp, 
                            CTCLResult& rResult, 
                            const char* parameter)   ; 
  std::string getOptionString () const   ; 
  virtual   int SetValue (CTCLInterpreter& rInterp, 
                          CTCLResult& rResult, 
                          const char* pvalue)   = 0 ;
  virtual std::string GetParameterFormat() = 0;

};

#endif
