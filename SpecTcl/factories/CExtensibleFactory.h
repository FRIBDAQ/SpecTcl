#ifndef __CEXTENSIBLEFACTORY_H
#define __CEXTENSIBLEFACTORY_H


/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_MAP
#include <map>
#ifndef __STL_MAP
#define __STL_MAP
#endif
#endif

#ifndef __CCREATOR_H
#include <CCreator.h>
#endif


/*!
  This is an extensible factory pattern realized as a template class.
  This allows a client to instantiate the factory for any class hierarchy.
  Supose I have a class hierarchy with a base class named george. 
  I can create an instance of this class factory by defining the following typedefs:

\verbatim

typedef CCreator<george>            CGeorgeCreator;
typedef CExtensibleFactory<george>  CGeorgeFactory;

\endverbatim
   Next; for each subclass of george, I derive a creator from 
   CGeorgeCreator. The creator is just a virtual constructor and
   generally looks like this:

\verbatim

george*
CGeorgeSubclassCreator::operator() {
   return new CGeorgeSubClass;
}

\endverbatim

   Having done all this, I need to generally wrap the
   extensible factory into a singleton and arrange for the
   factory to be populated with creators. Some of the creators
   (standard creators) could be created/installed in the singleton's
   getInstance method..here's more sample code:

\verbatim

class CGeorgeSingleton : public CGeorgeFactory {
private:
   static CGeorgeSingleton* theInstance;
private:
   CGeorgeSingleton();
public:
   CGeorgeSingleton* getInstance();
};
   
static CGeorgeSingleton* CGeorgeSingleton::theInstance(0);

CGeorgeSingleton*
CGeorgeSingleton::getInstance()
{
   if (!theInstance) {
      theInstance = new CGeorgeSingleton;
      theInstance.addCreator("subclass", new CGoergeSubclassCreator());
   }
   return theInstance;
}

\endverbatim


Then I can use the factory as follows:


\verbatim
..
CGeorgeSingleton* pFactory = CGeorgeSingleton::getInstance();
george*           pGeorge  = pFactory->create("subclass");
if (!pGeorge) {
cerr << "No creator for  " << "subclass" << endl;

..
\endverbatim
*/


template <class T>
class CExtensibleFactory
{
private:
  typedef  std::map<std::string, CCreator<T>* > CreatorMap;
  CreatorMap m_creators;

public:
  void addCreator(std::string type,
		  CCreator<T>* pCreator) {
    m_creators[type] = pCreator;
  }
  T* create(std::string type) {
    if(m_creators.find(type) == m_creators.end()) {
      return reinterpret_cast<T*>(NULL);
    } 
    else {
      return (*(m_creators.find(type)->second))();
    }
  }
  
};

#endif
