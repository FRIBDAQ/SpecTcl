/*!
ABC for creational classes that are instantiated and registered with
the CModuleCommand.  Together with the CModuleCommand this class
forms a recognizer/creator pattern.
*/
#ifndef __CMODULECREATOR_H  //Required for current class
#define __CMODULECREATOR_H

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

// Forward definitions:

class CModule;
class CTCLInterpreter;
class CTCLResult;
class CModuleCommand;
class CSegmentCreator;
class CSegmentUnpacker;

class CModuleCreator      
{
private:
    string          m_sType;            //!  Type of module created  
    CModuleCommand* m_pCreatorCommand;  //!<  The creator command.

public:
	// Constructors and other cannonical functions.
	
    CModuleCreator (const string& rType,
		    CModuleCommand* pCreatorCommand);
    virtual  ~CModuleCreator ( );
    CModuleCreator (const CModuleCreator& aCModuleCreator );
    CModuleCreator& operator= (const CModuleCreator& rhs);
    int operator== (const CModuleCreator& rhs) const;
    int operator!= (const CModuleCreator& rhs) const 
    {
	return !(operator==(rhs));
    }
// Selectors:

public:

          //Get accessor function for non-static attribute data member
  string getType() const
  { 
    return m_sType;
  }   


  // Class operations:

public:

   virtual   bool Match (const string& sModuletype)   ; // 
   virtual   CSegmentUnpacker* Create (CTCLInterpreter& rInterp, 
			      const string& rName) = 0;
   virtual string    TypeInfo() const= 0;
};

#endif
