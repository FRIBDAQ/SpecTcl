#ifndef __CSTRINGARRAYPARAM_H  //Required for current class
#define __CSTRINGARRAYPARAM_H

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

// Forward class definitions:

class CTCLInterpreter;
class CTCLResult;
 
/*!

*/
class CStringArrayparam  : public CConfigurationParameter        
{
private:
    STD(string)* m_aStrings;
    int m_nSize;
public:
    // Constructors and other 
    CStringArrayparam (const STD(string)& rKey, int nSize);
    virtual ~ CStringArrayparam ( ); 
    CStringArrayparam (const CStringArrayparam& rhs );
    CStringArrayparam& operator= (const CStringArrayparam& rhs);
    int operator== (const CStringArrayparam& rhs) const;
    int operator!=(const CStringArrayparam& rhs) const {
	return !(operator==(rhs));
    }

// Selectors:

public:

    const STD(string)* getStrings() const
    { 
	return m_aStrings;
    }

protected:

    void setStrings (STD(string)* aStrings)
   { 
	delete []m_aStrings;
	m_aStrings =aStrings;
    }   

  // Class operations:

public:

    STD(string)& operator[] (int n)   ; // 
    virtual   int SetValue (CTCLInterpreter& rInterp, CTCLResult& rResult, 
				    const char* pValue);
    virtual   STD(string) GetParameterFormat();
};

#endif
