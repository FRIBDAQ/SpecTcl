// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 
   


/*!
  \class CFitFactory

  Creator and dictionary of fits.

*/


#ifndef CFITFACTORY_H  //Required for current class
#define CFITFACTORY_H

//
// Include files:
//

#include <map>
#include <string>
#include <vector>
#include <Point.h>

// Forward class definitions (convert to includes if required):

class CFitCreator;
class CFit;

// The class itself.

class CFitFactory 
{
  // Public data types:
public:
  // Note that the iterators below will
  // 'point' to a pair<std::string,T*>.
  // e.g. a FitCreatorIterator points to a 
  // pair<std::string, CFitCreator*>
  //

  typedef std::map<std::string, CFitCreator*> FitCreatorMap;
  typedef FitCreatorMap::iterator  FitCreatorIterator;

  // Private member data.

private:  
  static FitCreatorMap m_mapCreators;

  // Note that all data are static, therefore
  // all functions are static too.

public:


  static void AddFitType (const std::string & rType, CFitCreator* pCreator)   ; 
  static CFit* Create (std::string sFitType, std::string sFitName, int id =  0)   ; 

  static FitCreatorIterator beginCreators();
  static FitCreatorIterator endCreators();
  static int    numberOfCreators();
  static FitCreatorIterator FindFitCreator (std::string sType)   ; 

};

#endif
