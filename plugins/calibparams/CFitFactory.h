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
