#include "TestFile.h"
#include "GaussianDistribution.h"
#include "Globals.h"

#include "MultiTestSource.h"

// Initializations.
CMultiTestSource* CMultiTestSource::m_pInstance = (CMultiTestSource*)kpNULL;

// Constructors: (Destructor is never called.)
CMultiTestSource::CMultiTestSource() :
  m_pDefaultTestSource((CTestFile*)kpNULL)
{
  m_mTestSources.clear();

  // For the default source:
  // The internal test data source is a set of 5 gaussian distributions which
  // produce a fixed size event.
  // The distributions are defined below:
  //                       Cent.  Sigma  Largest allowed value.
  static CGaussianDistribution d1(512.0, 128.0, 1024.0);
  static CGaussianDistribution d2(256.0,  64.0, 1024.0);
  static CGaussianDistribution d3(128.0,  32.0, 1024.0);
  static CGaussianDistribution d4( 64.0,  16.0, 1024.0);
  static CGaussianDistribution d5( 32.0,   8.0, 1024.0);

  m_pDefaultTestSource = new CTestFile;
  m_pDefaultTestSource->AddDistribution(d1);
  m_pDefaultTestSource->AddDistribution(d2);
  m_pDefaultTestSource->AddDistribution(d3);
  m_pDefaultTestSource->AddDistribution(d4);
  m_pDefaultTestSource->AddDistribution(d5);

  addTestSource("default", m_pDefaultTestSource);
  //m_pDefaultTestSource->Open("Testing", kacRead);
  //gpEventSource = m_pDefaultTestSource;
};

// Operators:
Bool_t CMultiTestSource::operator()(string sName) {
  return useTestSource(sName);
};

// Additional functions.
CMultiTestSource* CMultiTestSource::GetInstance() { // For singleton.
  if(m_pInstance == (CMultiTestSource*)kpNULL) {
    m_pInstance = new CMultiTestSource; // Calls constructor.
  }
  return m_pInstance;
}

Bool_t CMultiTestSource::addTestSource(string sName, CTestFile* pTestFile) {
  map<string, CTestFile*>::iterator i = m_mTestSources.find(sName);
  if(i != m_mTestSources.end()) { // Found.
    cerr << "Test (" << sName << ") already present in MultiTestSource map. No overwriting done." << endl;
    return kfFALSE;
  } else {
    m_mTestSources[sName] = pTestFile;
    return kfTRUE;
  }
};

CTestFile* CMultiTestSource::getTestSource(string sName) {
  map<string, CTestFile*>::iterator i = m_mTestSources.find(sName);
  if(i != m_mTestSources.end()) { // Found.
    return m_mTestSources[sName];
  } else {
    return (CTestFile*)kpNULL;
  }
};

CTestFile* CMultiTestSource::getDefaultTestSource() {
  return getTestSource("default");
};

Bool_t CMultiTestSource::useTestSource(string sName) {
  if(m_mTestSources.find(sName) != m_mTestSources.end()) { // Present.
    CTestFile* pTestFile = (CTestFile*)(m_mTestSources[sName]);
    pTestFile->Open("Testing", kacRead);
    gpEventSource = pTestFile;
    return kfTRUE;
  } else {
    cerr << "Error: Test(" << sName << ") not present in the MultiTestSource map. Cannot be opened." << endl;
    return kfFALSE;
  }
}

Bool_t CMultiTestSource::useDefaultTestSource() {
  m_pDefaultTestSource->Open("Testing", kacRead);
  gpEventSource = m_pDefaultTestSource;
  return kfTRUE;
}
