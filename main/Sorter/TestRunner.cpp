#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <string>
#include <iostream>
#include <TCLApplication.h>
#include <Analyzer.h>

using namespace std;

int main(int argc, char** argv)
{
  CppUnit::TextUi::TestRunner   
               runner; // Control tests.
  CppUnit::TestFactoryRegistry& 
               registry(CppUnit::TestFactoryRegistry::getRegistry());

  runner.addTest(registry.makeTest());

  bool wasSucessful;
  try {
    wasSucessful = runner.run("",false);
  } 
  catch(string& rFailure) {
    cerr << "Caught a string exception from test suites.: \n";
    cerr << rFailure << endl;
    wasSucessful = false;
  }
  return !wasSucessful;
}
// Stub stuff:

CTCLApplication* gpTCLApplication(0);
CAnalyzer*       gpAnalyzer(0);
CAnalyzer*       getAnalyzer() {return 0;}

class SpecTcl {
    static SpecTcl* m_pInstance;
public:
    SpecTcl() {}
    SpecTcl* getInstance();
    CAnalyzer* GetAnalyzer();
    
};

SpecTcl* SpecTcl::getInstance() {
        if (!m_pInstance) {
            m_pInstance = new SpecTcl;
        }
        return m_pInstance;
    }
    
CAnalyzer* SpecTcl::GetAnalyzer() {return 0;}


SpecTcl* SpecTcl::m_pInstance(0);