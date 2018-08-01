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
#ifndef CSTRINGARRAYPARAM_H  //Required for current class
#define CSTRINGARRAYPARAM_H

                               //Required for base classes
#include "CConfigurationParameter.h"
#include <string>

// Forward class definitions:

class CTCLInterpreter;
class CTCLResult;
 
/*!

*/
class CStringArrayparam  : public CConfigurationParameter        
{
private:
    std::string* m_aStrings;
    int m_nSize;
public:
    // Constructors and other 
    CStringArrayparam (const std::string& rKey, int nSize);
    virtual ~ CStringArrayparam ( ); 
    CStringArrayparam (const CStringArrayparam& rhs );
    CStringArrayparam& operator= (const CStringArrayparam& rhs);
    int operator== (const CStringArrayparam& rhs) const;
    int operator!=(const CStringArrayparam& rhs) const {
	return !(operator==(rhs));
    }

// Selectors:

public:

    const std::string* getStrings() const
    { 
	return m_aStrings;
    }

protected:

    void setStrings (std::string* aStrings)
   { 
	delete []m_aStrings;
	m_aStrings =aStrings;
    }   

  // Class operations:

public:

    std::string& operator[] (int n)   ; // 
    virtual   int SetValue (CTCLInterpreter& rInterp, CTCLResult& rResult, 
				    const char* pValue);
    virtual   std::string GetParameterFormat();
};

#endif
