#include "CModuleDictionary.h"    				
#include "CModule.h"
/*!    Constructs a module dictionary.  In fact this is a No-op
    for this class as all the work is done by the default
    constructor of the map class.
*/
CModuleDictionary::CModuleDictionary ()
{   
} 
/*!
    Destroys a module dictionary.  This too is a no-op since the map destructor
    does all that it needs to do, and it's the responsibility of the creators
    of a module to destroy them.
*/
CModuleDictionary::~CModuleDictionary ( ) 
{
}

/*!
    Copy constructor.  Constructs a new module dictionary as a copy of the
    rhs parameter.
    \param rhs (const CModuleDictionary& [in]): the reference from which the new
	 object will be created.
*/
CModuleDictionary::CModuleDictionary (const CModuleDictionary& rhs) :
     m_Modules(rhs.m_Modules)
{

} 

/*!
  Assignment:  Replaces the current object with the contents of the rhs parameter.
  \param rhs (const CModuleDictionary& [in]): Reference to the object that will be
      copied into this object.  \return Reference to this to support operator chaining.
*/
CModuleDictionary& 
CModuleDictionary::operator= (const CModuleDictionary& rhs)
{ 
    if(this != &rhs) {
	m_Modules = rhs.m_Modules;
    }
    return *this;
}

/*!
    Equality comparison... This delegates to the map operator==:
    \param rhs (const CModuleDictionary& [in]): Reference to the object that will
	be compared to this.
    \return 1 - Equal or 0 if not.
*/
int 
CModuleDictionary::operator== (const CModuleDictionary& rhs) const
{ 
    return (m_Modules == rhs.m_Modules);
}

// Functions for class CModuleDictionary

/*!  

Adds a module to the dictionary.
If the module exists a string exception
is thrown indicating that fact.  In practice,
the module command will check for existence
prior to attempting to add.
\param pModule (CModule* [in]):  The module to add.

*/
void 
CModuleDictionary::Add(CSegmentUnpacker* pModule)  
{ 
    string name = pModule->getName();
    if(Find(name) == end()) {
	m_Modules[name] = pModule;
    }
    else {
	string error("A module named: ");
	error+= name;
	error+= " already exists.";
	throw error;
    }
}  

/*!  Function: 	
   

Returns an iterator to the module with the
name given.  If there is no match, the returned
value will be identical to that produced by end().
\param rName (const string& [in]): The name of the module to hunt for.
\return An iterator to the found module or end()  the module does not
   exist.

*/
CModuleDictionary::ModuleIterator 
CModuleDictionary::Find(const string& rName)  
{ 
    return m_Modules.find(rName);
}  

/*!  

Deletes a module given an iterator to it.
    \param iModule (ModuleIterator [in]):

*/
void 
CModuleDictionary::Delete(CModuleDictionary::ModuleIterator iModule)  
{ 
    m_Modules.erase(iModule);
}  

/*!  

Returns an iterator to the 'first' module 
in the dictionary. The iterator 'points' to 
a pair<string, CModule*>

*/
CModuleDictionary::ModuleIterator 
CModuleDictionary::begin()  
{ 
    return m_Modules.begin();
}  

/*!  

Returns an end of iteration value.

*/
CModuleDictionary::ModuleIterator
CModuleDictionary::end()  
{ 
    return m_Modules.end();
}  

/*!  Function: 	
  

Returns the number of items in the dictionary.

*/
int 
CModuleDictionary::size()  
{
     return m_Modules.size();
}
