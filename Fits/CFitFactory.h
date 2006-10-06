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


#ifndef __CFITFACTORY_H  //Required for current class
#define __CFITFACTORY_H

//
// Include files:
//

#ifndef __STL_MAP
#include <map>
#ifndef __STL_MAP
#define __STL_MAP
#endif
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif


#ifndef __POINT_H
#include <Point.h>
#endif

// Forward class definitions (convert to includes if required):

class CFitCreator;
class CFit;

// The class itself.

class CFitFactory 
{
  // Public data types:
public:
  // Note that the iterators below will
  // 'point' to a pair<STD(string),T*>.
  // e.g. a FitCreatorIterator points to a 
  // pair<STD(string), CFitCreator*>
  //

  typedef STD(map)<STD(string), CFitCreator*> FitCreatorMap;
  typedef FitCreatorMap::iterator  FitCreatorIterator;

  // Private member data.

private:  
  static FitCreatorMap m_mapCreators;

  // Note that all data are static, therefore
  // all functions are static too.

public:


  static void AddFitType (const STD(string) & rType, CFitCreator* pCreator)   ; 
  static CFit* Create (STD(string) sFitType, STD(string) sFitName)   ; 

  static FitCreatorIterator beginCreators();
  static FitCreatorIterator endCreators();
  static int    numberOfCreators();
  static FitCreatorIterator FindFitCreator (STD(string) sType)   ; 

};

#endif
