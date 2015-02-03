/*!
ABC for creational classes that are instantiated and registered with
the CModuleCommand.  Together with the CModuleCommand this class
forms a recognizer/creator pattern.
*/
#ifndef __CMODULECREATOR_H  //Required for current class
#define __CMODULECREATOR_H

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
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
    STD(string)          m_sType;            //!  Type of module created  
    CModuleCommand* m_pCreatorCommand;  //!<  The creator command.

public:
	// Constructors and other cannonical functions.
	
    CModuleCreator (const STD(string)& rType,
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
  STD(string) getType() const
  { 
    return m_sType;
  }   


  // Class operations:

public:

   virtual   bool Match (const STD(string)& sModuletype)   ; // 
   virtual   CSegmentUnpacker* Create (CTCLInterpreter& rInterp, 
			      const STD(string)& rName) = 0;
   virtual STD(string)    TypeInfo() const= 0;
};

#endif
