#ifndef __MULTITESTSOURCE_H
#define __MULTITESTSOURCE_H

#ifndef __STL_MAP
#include <map>
#define __STL_MAP
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __FILE_H
#include "File.h"
#endif

#ifndef __TESTFILE_H
#include "TestFile.h"
#endif

// Class. (Singleton Pattern - ensures only a single instance at most.)
class CMultiTestSource : public CFile {
  // Attributes
 private:
  static CMultiTestSource* m_pInstance;
  STD(map)<STD(string), CTestFile*> m_mTestSources;
  CTestFile* m_pDefaultTestSource;

 protected:
  // Constructors:
  CMultiTestSource();
  //~CMultiTestSource(); // Destructor is never called for a singleton.

 public:
  // Operators:
  Bool_t operator()(STD(string)); // Uses the requested test source.

  // Additional functions:
  static CMultiTestSource* GetInstance(); // For the singleton.

  Bool_t addTestSource(STD(string), CTestFile*);
  CTestFile* getTestSource(STD(string));
  CTestFile* getDefaultTestSource();
  Bool_t useTestSource(STD(string));
  Bool_t useDefaultTestSource();
  void   destroyingTestSource(CTestFile* source);
}; // CMultiTestSource.

#endif
