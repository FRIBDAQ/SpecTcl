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

/*!
ABC for creational classes that are instantiated and registered with
the CModuleCommand.  Together with the CModuleCommand this class
forms a recognizer/creator pattern.
*/
#ifndef CMODULECREATOR_H  //Required for current class
#define CMODULECREATOR_H

#include <string>

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
    std::string          m_sType;            //!  Type of module created  
    CModuleCommand* m_pCreatorCommand;  //!<  The creator command.

public:
	// Constructors and other cannonical functions.
	
    CModuleCreator (const std::string& rType,
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
  std::string getType() const
  { 
    return m_sType;
  }   


  // Class operations:

public:

   virtual   bool Match (const std::string& sModuletype)   ; // 
   virtual   CSegmentUnpacker* Create (CTCLInterpreter& rInterp, 
			      const std::string& rName) = 0;
   virtual std::string    TypeInfo() const= 0;
};

#endif
