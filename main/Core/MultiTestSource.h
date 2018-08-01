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


#ifndef MULTITESTSOURCE_H
#define MULTITESTSOURCE_H

#include <map>
#include <string>
#include "File.h"
#include "TestFile.h"

// Class. (Singleton Pattern - ensures only a single instance at most.)
class CMultiTestSource : public CFile {
  // Attributes
 private:
  static CMultiTestSource* m_pInstance;
  std::map<std::string, CTestFile*> m_mTestSources;
  CTestFile* m_pDefaultTestSource;

 protected:
  // Constructors:
  CMultiTestSource();
  //~CMultiTestSource(); // Destructor is never called for a singleton.

 public:
  // Operators:
  Bool_t operator()(std::string); // Uses the requested test source.

  // Additional functions:
  static CMultiTestSource* GetInstance(); // For the singleton.

  Bool_t addTestSource(std::string, CTestFile*);
  CTestFile* getTestSource(std::string);
  CTestFile* getDefaultTestSource();
  Bool_t useTestSource(std::string);
  Bool_t useDefaultTestSource();
  void   destroyingTestSource(CTestFile* source);
}; // CMultiTestSource.

#endif
