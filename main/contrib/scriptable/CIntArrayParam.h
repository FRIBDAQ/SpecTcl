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
//! \class: CIntArrayParam           
//! \file:  .h
// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef CINTARRAYPARAM_H  //Required for current class
#define CINTARRAYPARAM_H

#include "CConfigurationParameter.h"
#include <string>

// Forward class definitions:

class CTCLInterpreter;
class CTCLResult;
/*!
Encapsulates a parameter that is aray valued.  An example of an array
valued parameter is a set of pedestals one per digitizer channel.  
The array is passed in as a welll formed TCL list, each of whose elements
will evailuate to an integer.
*/
class CIntArrayParam : public CConfigurationParameter     
{
private:
  
  // Private Member data:
  int  m_nSize;         //! Size of the array.  
  int* m_aValues;      //! Fixed size array for the values.  
  bool m_fCheckRange;  //! true if range checking is to be enabled.  
  int  m_nLow;          //! Smallest allowed value for the parameter.    
  int  m_nHigh;         //! largest allowed value for an element of the array.  
   
public:
// Constructors and other canonical members:

  CIntArrayParam (const std::string& rName, 
                  unsigned int size,
                  int nDefault = 0);
  CIntArrayParam (const std::string&, 
                  unsigned int size,
                  int nLow, 
                  int nHigh, 
                  int nDefault = 0);
  virtual  ~ CIntArrayParam ( ); 

  CIntArrayParam (const CIntArrayParam& aCIntArrayParam );
  CIntArrayParam& operator= (const CIntArrayParam& aCIntArrayParam);
  int operator== (const CIntArrayParam& aCIntArrayParam) const;
  int operator!= (const CIntArrayParam& rhs) const
  {
      return !operator==(rhs);
  }
// Selectors:

public:
  int getSize() const
  { 
    return m_nSize;
  }  
  const int* getValues() const
  { 
    return m_aValues;
  }  
  bool getCheckRange() const
  {
    return m_fCheckRange;
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

  void setSize (const int am_nSize)
  { 
    m_nSize = am_nSize;
  }  
  void setValues (int* am_aValues)
  { 
    m_aValues = am_aValues;
  }  
  void setCheckRange (const bool am_fCheckRange)
  { 
    m_fCheckRange = am_fCheckRange;
  }  
  void setLow (const int am_nLow)
  { 
    m_nLow = am_nLow;
  }  
  void setHigh (const int am_nHigh)
  { m_nHigh = am_nHigh;
  }   

  // Class operations:

public:

  int&        operator[] (int n);
  virtual int SetValue (CTCLInterpreter& rInterp, 
                        CTCLResult& rResult, 
                        const char* pValues);
  virtual std::string GetParameterFormat();
  void setRange(int nLow, int nHigh)
  {
    m_nLow  = nLow;
    m_nHigh = nHigh;
    m_fCheckRange = true;
  }

  // utility:
protected:
  void SetupArray(int nDefault);
  void CopyArray(const CIntArrayParam& rhs);
  void SetupStringValue();
};

#endif
