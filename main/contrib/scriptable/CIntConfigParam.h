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
#ifndef __CINTCONFIGPARAM_H  //Required for current class
#define __CINTCONFIGPARAM_H


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif


//
// Include files:
//

                               //Required for base classes
#ifndef __CCONFIGURATIONPARAMETER_H     //CConfigurationParameter
#include "CConfigurationParameter.h"
#endif
 
/*!
Represents an integer configuration parameter
Integer configuration parametes appear as a keyword value
pair.  E.g.:
\verbatim
-vsn 5
\endverbatim
might set a FERA virtual slot number to 5.

*/
class CIntConfigParam  : public CConfigurationParameter        
{
private:
  
  // Private Member data:
  bool m_fCheckrange;  //!< True if value has range limits.  
  int m_nLow;          //!< Lowest allowed value for parameter.  
  int m_nHigh;         //!< Highest allowed value for parameter.  
  int m_nValue;        //!< Current parameter value as int.
public:
  //  Constructors.  The first constructor is without
  //  range checking, the second with.

  CIntConfigParam (const std::string& rName, int nDefault=0);
  CIntConfigParam (const std::string& rName,
                   int nLow, int nHigh, int nDefault=0);
  // Destructor:
  virtual ~CIntConfigParam ( );  

             //Copy Constructor 
  CIntConfigParam (const CIntConfigParam& aCIntConfigParam );

             //Operator= Assignment Operator 
  CIntConfigParam& operator= (const CIntConfigParam& aCIntConfigParam);
 
            //Operator== Equality Operator 
  int operator== (const CIntConfigParam& aCIntConfigParam) const;

    // Non equal compare:

  int operator!=(const CIntConfigParam& rhs) const 
  {
      return !operator==(rhs);
  }
// Selectors:

public:
  bool getCheckrange() const
  { 
    return m_fCheckrange;
  }  
  int getLow() const
  { 
    return m_nLow;
  }  
  int getHigh() const
  { 
    return m_nHigh;
  }   


  // Attribute mutators:

protected:

  void setCheckrange (const bool am_fCheckrange)
  { 
    m_fCheckrange = am_fCheckrange;
  }  
  void setLow (const int am_nLow)
  { 
    m_nLow = am_nLow;
  }  
  void setHigh (const int am_nHigh)
  { 
    m_nHigh = am_nHigh;
  }   

  // Class operations:

public:

  int getOptionValue ();        //!< Parameter value as int.
  virtual   int SetValue (CTCLInterpreter& rInterp, 
                          CTCLResult& rResult, 
                          const char* pValue)   ; //!< Parse/validate

  virtual std::string GetParameterFormat();
  void setRange(int nLo, int nHi) {
      m_nLow  = nLo;
      m_nHigh = nHi;
      m_fCheckrange = true;
  }
};

#endif
