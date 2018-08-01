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

  typedef std::map<std::string, CFit*>        FitMap;
  typedef FitMap::iterator          FitIterator;

  

  // Private member data.

private:  
  static FitCreatorMap m_mapCreators;
  static FitMap        m_mapDefinedFits;

  // Note that all data are static, therefore
  // all functions are static too.

public:


  static void AddFitType (const std::string & rType, CFitCreator* pCreator)   ; 
  static CFit* Create (std::string sFitType, std::string sFitName)   ; 
  static bool Delete (std::string sName)   ; 
  static bool Perform (std::string sName)   ; 
  static bool AddPoints (std::string sName,
			 std::vector<FPoint> vPoints)   ; 
  static double Evaluate (std::string sName, double x)   ; 
  static FitCreatorIterator beginCreators();
  static FitCreatorIterator endCreators();
  static int    sizeCreators();
  static FitCreatorIterator FindFitCreator (std::string sType)   ; 
  static int size ()   ; 
  static FitIterator begin ()   ; 
  static FitIterator end ()   ; 
  static FitIterator FindFit (std::string sName) ; 


};

#endif
