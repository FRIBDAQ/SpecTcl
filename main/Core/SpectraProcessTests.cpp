
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <config.h>

#include "SpectraProcess.h"
#include "SpecTcl.h"

#include <Asserts.h>

#include <unistd.h>

#include <memory>


using namespace std;

class SpectraProcessTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SpectraProcessTests);
    CPPUNIT_TEST(generatePath_0);
    CPPUNIT_TEST_SUITE_END();


public:

    void generatePath_0() {
        Spectra::CSpectraProcess process;

        setenv("SPECTRA_EXECUTABLE_PATH", "/test/path", 1);
        EQMSG("Correct path is generated",
              string("/test/path"), process.generatePath() );
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(SpectraProcessTests);



