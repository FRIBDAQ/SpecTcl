

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <Asserts.h>

#include <DisplayFactory.h>
#include <NullDisplay.h>

#include <iostream>

using namespace std;

class DisplayFactoryTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(DisplayFactoryTests);
    CPPUNIT_TEST(addCreator_0);
    CPPUNIT_TEST(addCreator_1);
    CPPUNIT_TEST(getCreator_0);
    CPPUNIT_TEST(getCreator_1);
    CPPUNIT_TEST(removeCreator_0);
    CPPUNIT_TEST(removeCreator_1);
    CPPUNIT_TEST(create_0);
    CPPUNIT_TEST(create_1);
    CPPUNIT_TEST_SUITE_END();

private:
    CDisplayFactory *pFactory;
    CDisplayCreator *pCreator;

public:
    void setUp() {
        pFactory = new CDisplayFactory;
        pCreator = new CNullDisplayCreator;
    }

    void tearDown() {
        delete pFactory;
        if (pCreator) {
            delete pCreator;
        }
    }


    void addCreator_0 () {
        bool result = pFactory->addCreator("null", *pCreator);
        EQMSG("Successful additon returns true to signal ownership transfer",
              true, result);
        pCreator = NULL; // make sure we don't double free in tearDown
    }

    void addCreator_1 () {
        bool result = pFactory->addCreator("null", *pCreator);
        result = pFactory->addCreator("null", *pCreator);
        EQMSG("Adding creator for preexisting type returns false",
              false, result);
        pCreator = NULL;
    }

    void getCreator_0 () {
        pFactory->addCreator("null", *pCreator);
        EQMSG("Getting creator should return the original creator",
              pCreator, pFactory->getCreator("null"));
        pCreator = NULL;
    }

    void getCreator_1 () {
        EQMSG("Getting creator non existent type returns 0",
              (CDisplayCreator*)NULL, pFactory->getCreator("i don't exist"));

    }

    void removeCreator_0 () {
        EQMSG("Removing creator for non existent type returns 0",
              (CDisplayCreator*)NULL, pFactory->removeCreator("i don't exist"));
    }


    void removeCreator_1 () {
        pFactory->addCreator("null", *pCreator);
        EQMSG("Removing creator for existing type returns original pointer",
              pCreator, pFactory->removeCreator("null"));
    }

    void create_0 () {
        EQMSG("Creating type without creator returns null",
              (CDisplay*)NULL, pFactory->create("i don't exist"));
    }

    void create_1 () {
        pFactory->addCreator("null", *pCreator);
        ASSERTMSG("Creating type with creator succeeds",
              pFactory->create("null") != NULL);

        pCreator = NULL;
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(DisplayFactoryTests);
