#ifndef __CMODULE_H  //Required for current class
#define __CMODULE_H

#ifndef __CTCLPROCESSOR_H
#include "TCLProcessor.h"
#endif

#ifndef __STL_STRING
#include <string>        //Required for include files  
#define __STL_STRING
#endif

#ifndef __STL_MAP
#include <map>
#define __STL_MAP
#endif

#ifndef __TRANSLATORPOINTER_H
#include "TranslatorPointer.h"
#endif

// Forward class definitions:

class CConfigurationParameter;
class CBoolConfigParam;
class CStringConfigParam;
class CIntConfigParam;
class CIntArrayParam;
class CStringArrayparam;
class CTCLInterpreter;
class CTCLResult;
class CAnalyzer;
class CHistogrammer;
class CBufferDecoder;
class CEvent;

/*!
Abstract base class representing unpackers for data
put in the event stream by a module.  Module objects
have two stereotypes:
-  They are commands that can be used to configure
    the module (for example assign parameter names
    to the 

*/
class CModule : public CTCLProcessor     
{
public:
  typedef map<string, CConfigurationParameter*> Configuration;
    typedef Configuration::iterator               ConfigurationIterator;
private:
    string 		m_sName;  			//!< Module name.
    Configuration   m_Parameters;            //!< Current module configuration.

public:
    // Constructors and other cannonical operations:
    
    CModule (const string& rName,
	         CTCLInterpreter& rInterp);
    virtual  ~ CModule ( );
private:
    CModule (const CModule& aCModule );
    CModule& operator= (const CModule& aCModule);
    int operator== (const CModule& aCModule) const;
    int operator!= (const CModule& rhs) const;
public:

// Selectors:

public:

          //Get accessor function for non-static attribute data member
    string getName() const
    { 
	return m_sName;
    }  
    Configuration getParameters() const
    {
	return m_Parameters;
    }

// Attribute mutators:

protected:

          //Set accessor function for non-static attribute data member
	  
  void setName (const string am_sName)
  { 
      m_sName = am_sName;
  }
  void setParameters(const Configuration& rConfig) {
      m_Parameters = rConfig;
    }


  // Class operations:

public:
    virtual string getType() const = 0;
    virtual   int operator() (CTCLInterpreter& rInterp, CTCLResult& rResult,
					int argc, char** argv)   ;  
    virtual   int Configure (CTCLInterpreter& rInterp, CTCLResult &rResult, 
				    int argc, char** argv)   ; 
    virtual   int ListConfig (CTCLInterpreter& rInterp, CTCLResult& rResult, 
				    int argc, char** argv)   ;  
    virtual   int OtherSubcommands (CTCLInterpreter& rInterp, 
						  CTCLResult& rResult, 
						  int argc, char** argv)   ; 
    virtual   TranslatorPointer<UShort_t> 
      Unpack (TranslatorPointer<UShort_t> pBuffer, 
	      CEvent& rEvent,
	      CAnalyzer& rAnalyzer, 
	      CBufferDecoder& rDecoder)  = 0 ; 
    virtual   void Setup (CAnalyzer&  rAnalyzer, 
				  CHistogrammer& rHistogrammer)   = 0;  
    CIntConfigParam* AddIntParam (const string &rName, int def = 0)   ; 
    CBoolConfigParam* AddBoolParam (const string& rName, 
						 bool def = false)   ; 
    CIntArrayParam* AddIntArrayParam (const string&rName, int nSize,
						      int def = 0)   ; 
    CStringConfigParam* AddStringParam (const string& rName)   ; 
    CStringArrayparam* AddStringArrayParam (const string& rName, 
								   int nSize)   ; 
    void AddParameter(CConfigurationParameter* pParam);
    Configuration::iterator begin();
    Configuration::iterator end();
    int                           size();
    virtual   string Usage ()   ; // 

};

#endif
