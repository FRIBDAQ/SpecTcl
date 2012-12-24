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

class CCalibFitCreator;
class CCalibFit;

// The class itself.

class CCalibFitFactory 
{
  // Public data types:
public:
  // Note that the iterators below will
  // 'point' to a pair<STD(string),T*>.
  // e.g. a FitCreatorIterator points to a 
  // pair<STD(string), CCalibFitCreator*>
  //

  typedef STD(map)<STD(string), CCalibFitCreator*> FitCreatorMap;
  typedef FitCreatorMap::iterator  FitCreatorIterator;

  typedef STD(map)<STD(string), CCalibFit*>        FitMap;
  typedef FitMap::iterator          FitIterator;

  

  // Private member data.

private:  
  static FitCreatorMap m_mapCreators;
  static FitMap        m_mapDefinedFits;

  // Note that all data are static, therefore
  // all functions are static too.

public:


  static void AddFitType (const STD(string) & rType, CCalibFitCreator* pCreator)   ; 
  static CCalibFit* Create (STD(string) sFitType, STD(string) sFitName)   ; 
  static bool Delete (STD(string) sName)   ; 
  static bool Perform (STD(string) sName)   ; 
  static bool AddPoints (STD(string) sName,
			 STD(vector)<FPoint> vPoints)   ; 
  static double Evaluate (STD(string) sName, double x)   ; 
  static FitCreatorIterator beginCreators();
  static FitCreatorIterator endCreators();
  static int    sizeCreators();
  static FitCreatorIterator FindFitCreator (STD(string) sType)   ; 
  static int size ()   ; 
  static FitIterator begin ()   ; 
  static FitIterator end ()   ; 
  static FitIterator FindFit (STD(string) sName) ; 


};

#endif
