/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  wfcmdtests
 *  @brief: Test the spectc::serial::waveform command.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CWaveformCommand.h"
#include <TCLInterpreter.h>
#include <TCLException.h>
#define private public
#include "CWaveFormDictionary.h"
#undef private

#include "SpecTcl.h"
#include "CWaveForm.h"
#include <TclDict.h>
#include <tcl.h>

class WFCmdTests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(WFCmdTests);
    CPPUNIT_TEST(create_1);
    CPPUNIT_TEST(create_2);
    CPPUNIT_TEST(create_3);
    CPPUNIT_TEST(create_4);

    CPPUNIT_TEST(list_1);
    CPPUNIT_TEST(list_2);
    CPPUNIT_TEST(list_3);
    CPPUNIT_TEST(list_4);
    CPPUNIT_TEST(list_5);

    CPPUNIT_TEST(get_1);
    CPPUNIT_TEST(get_2);
    CPPUNIT_TEST(get_3);
    CPPUNIT_TEST(get_4);

    CPPUNIT_TEST(metadata_1);
    CPPUNIT_TEST(metadata_2);

    CPPUNIT_TEST(mdset_1);
    CPPUNIT_TEST(mdset_2);
    CPPUNIT_TEST(mdset_3);
    CPPUNIT_TEST(mdset_4);
    CPPUNIT_TEST(mdset_5);

    CPPUNIT_TEST(mdget_1);
    CPPUNIT_TEST(mdget_2);
    CPPUNIT_TEST(mdget_3);
    CPPUNIT_TEST(mdget_4);
    
    CPPUNIT_TEST(resize_1);
    CPPUNIT_TEST(resize_2);
    CPPUNIT_TEST(resize_3);
    CPPUNIT_TEST(resize_4);

    // Tests fits subcommand (ISsue #212)

    CPPUNIT_TEST(fits_1);   // too few params.
    CPPUNIT_TEST(fits_2);   // too many params.
    CPPUNIT_TEST(fits_3);   // no such waveform.
    CPPUNIT_TEST(fits_4);   // No fits present.
    CPPUNIT_TEST(fits_5);   // There's a fit.   
    CPPUNIT_TEST(fits_6);   // The fit list is correct.
    CPPUNIT_TEST(fits_7);   // pattern defaults to *

    // test waveform getall sub-command (Issue #212).

    CPPUNIT_TEST(getall_1);  // Too few parameters.
    CPPUNIT_TEST(getall_2);  // One waveform, no fits.
    CPPUNIT_TEST(getall_3);  // One waveform 1 fit.
    CPPUNIT_TEST(getall_4);  // One waveform more than 1 fit.
    CPPUNIT_TEST(getall_5);  // Two waveforms neither has fits.
    CPPUNIT_TEST(getall_6);  // Two waveforms first has a fit.
    CPPUNIT_TEST(getall_7);  // Two waveforms second has two fits.
    CPPUNIT_TEST(getall_8);  // Two waveforms first as a fit second has two fits.
    CPPUNIT_TEST(getall_9);  // no such waveform error.

    CPPUNIT_TEST_SUITE_END();

    // test methods
private:
    void create_1();
    void create_2();
    void create_3();
    void create_4();
    void create_5();

    void list_1();
    void list_2();
    void list_3();
    void list_4();
    void list_5();

    void get_1();
    void get_2();
    void get_3();
    void get_4();

    void metadata_1();
    void metadata_2();

    void mdset_1();
    void mdset_2();
    void mdset_3();
    void mdset_4();
    void mdset_5();

    void mdget_1();
    void mdget_2();
    void mdget_3();
    void mdget_4();

    void resize_1();
    void resize_2();
    void resize_3();
    void resize_4();

    void fits_1();
    void fits_2();
    void fits_3();
    void fits_4();
    void fits_5();
    void fits_6();
    void fits_7();

    void getall_1();
    void getall_2();
    void getall_3();
    void getall_4();
    void getall_5();
    void getall_6();
    void getall_7();
    void getall_8();
    void getall_9();

private:    // utilities
    std::pair<CWaveform*, CWaveform*>  makeWfPair();
// Test objects:
private:
    CTCLInterpreter* m_pInterp;
    CWaveformCommand* m_pCommand;
public:
    void setUp() {
        m_pInterp = new CTCLInterpreter;
        m_pCommand = new CWaveformCommand(*m_pInterp);
    }
    void tearDown() {
        delete m_pCommand;
        delete m_pInterp;

        // Also destroy the waveform dict:

        delete CWaveFormDictionary::m_pInstance;
        CWaveFormDictionary::m_pInstance = nullptr;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(WFCmdTests);

// create tests:

void
WFCmdTests::create_1() {
    // explicit create subcommand:

    // successful.
    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform create test 100")
    );  
    // The waveform was created and has the right size

    auto wf = SpecTcl::getInstance()->findWaveform("test");
    ASSERT(wf);
    EQ(size_t(100), wf->size());

}

void
WFCmdTests::create_2() {
    // implicit create:

    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform test 100")
    );

    auto wf = SpecTcl::getInstance()->findWaveform("test");
    ASSERT(wf);
    EQ(size_t(100), wf->size());
}

void WFCmdTests::create_3() {
    // not enough parameters.

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcL::serial::waveform create test"),
        CTCLException
    );
}
void WFCmdTests::create_4() {
    // too many parameters.

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcL::serial::waveform create test 100 junk"),
        CTCLException
    );
}
void WFCmdTests::create_5() {
    // duplicate should fail:

    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform create test 100")
    );

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spctcl::serial::waveform create test 150"),    // already exists.
        CTCLException
    );

    // THe old one is still there and has correct size.

    auto wf = SpecTcl::getInstance()->findWaveform("test");
    ASSERT(wf);
    EQ(size_t(100), wf->size());
}

/// tests for list:

void WFCmdTests::list_1() {
    // no waveforms gives empty list result both with and without pattern:

    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform list *")
    );

    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GetResultString();
    EQ(0, result.llength());

    // NOw with the default "*" pattern:

    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform list")
    );
    result = m_pInterp->GetResultString();
    EQ(0, result.llength());
}
void WFCmdTests::list_2() {
    // too many command params:

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform list a b"),
        CTCLException
    );
}
void WFCmdTests::list_3() {
    // List when one waveform is present:

    m_pInterp->GlobalEval("spectcl::serial::waveform create test  100"); 
    m_pInterp->GlobalEval("spectcl::serial::waveform list");

    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GetResultString();
    EQ(1, result.llength());    // There's one dict.

    CTCLObject dict = result.lindex(0);
    dict.Bind(*m_pInterp);

    // Should look like:  {name test samples 100 metadata {}} since dicts preserve order.
    // shimmer into a list and see:

    EQ(6, dict.llength());
    EQ(std::string("name"), std::string(dict.lindex(0)));
    EQ(std::string("test"), std::string(dict.lindex(1)));
    EQ(std::string("samples"), std::string(dict.lindex(2)));
    EQ(std::string("100"), std::string(dict.lindex(3)));
    EQ(std::string("metadata"), std::string(dict.lindex(4)));
    EQ(std::string(""), std::string(dict.lindex(5)));

    // Assume that other than possibly the metadata, the list format is correct...
    // can we pattern match it?

    m_pInterp->GlobalEval("spectcl::serial::waveform list test");
    result = m_pInterp->GetResultString();
    EQ(1, result.llength());    // There's one dict.

    m_pInterp->GlobalEval("spectcl::serial::waveform list testing");
    result = m_pInterp->GetResultString();
    EQ(0, result.llength());    // No match.

}

void WFCmdTests::list_4() {
    // If there are two wf's I can see them both and select them.
    m_pInterp->GlobalEval("spectcl::serial::waveform create test1  100"); 
    m_pInterp->GlobalEval("spectcl::serial::waveform create test2  100"); 

    m_pInterp->GlobalEval("spectcl::serial::waveform list");   // Matches both:

    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GetResultString();
    EQ(2, result.llength());

    // Using partial pattern match:

    m_pInterp->GlobalEval("spectcl::serial::waveform list test*");   // Matches both:
    result = m_pInterp->GetResultString();
    EQ(2, result.llength());

    // match only test1:

    m_pInterp->GlobalEval("spectcl::serial::waveform list *1");  // only one match.
    result = m_pInterp->GetResultString();
    EQ(1, result.llength());

    CTCLObject dict;
    dict.Bind(*m_pInterp);
    dict = result.lindex(0);
    EQ(std::string("test1"),std::string(dict.lindex(1)));    // Name is correct.

}

void WFCmdTests::list_5() {
    // Waveform with metadata.
    // note we manually add it:

    m_pInterp->GlobalEval("spectcl::serial::waveform create test  100"); 
    auto wf = SpecTcl::getInstance()->findWaveform("test");
    wf->setMetadata("frequency", "250MHz");

    // Now the list:

    m_pInterp->GlobalEval("spectcl::serial::waveform list");
    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GetResultString();

    CTCLObject wfdict;
    wfdict.Bind(*m_pInterp);
    wfdict = result.lindex(0);   // The matched dict.

    CTCLObject mddict;
    mddict.Bind(*m_pInterp);
    mddict = wfdict.lindex(5);   // Metadata dict.

    EQ(2, mddict.llength());    // frequency 250MHz:
    EQ(std::string("frequency"), std::string(mddict.lindex(0)));
    EQ(std::string("250MHz"), std::string(mddict.lindex(1)));
}

// tests of the get subcommand.

void WFCmdTests::get_1() {
    // Single trace returned:

    m_pInterp->GlobalEval("spectcl::serial::waveform create test 100");
    
    // Put in a ramp trace:

    std::vector<uint16_t> trace;
    for (int i =0; i < 100; i++) {
        trace.push_back(i);
    }

    auto wf = SpecTcl::getInstance()->findWaveform("test");
    wf->update(trace.data());

    // Now get it:

    CPPUNIT_ASSERT_NO_THROW(m_pInterp->GlobalEval("spectcl::serial::waveform get test"));

    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GetResultString();
    EQ(1, result.llength());    // One trace.
    CTCLObject t;
    t.Bind(*m_pInterp);                     
    t = result.lindex(0);                      // {name {points...}}
    EQ(3, t.llength());

    std::string name = std::string(t.lindex(0));
    EQ(std::string("test"),name);
    t = t.lindex(1);
    EQ(100, t.llength());

    for (int i = 0; i < 100; i++) {
        int v = t.lindex(i);
        EQ(trace[i], uint16_t(v));
    }

}
void WFCmdTests::get_2() {
    // no such waveform.
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform get test"),
        CTCLException
    );
}
void WFCmdTests::get_3() {
    // Get two waveforms.
    m_pInterp->GlobalEval("spectcl::serial::waveform create test1 100");
    m_pInterp->GlobalEval("spectcl::serial::waveform create test2 100");

    std::vector<uint16_t> trace;
    for (int i=0; i < 100; i++) {
        trace.push_back(i);
    }
    SpecTcl::getInstance()->findWaveform("test1")->update(trace.data());

    for (int i =0; i < 100; i++) {
        trace[i] = 100-i;
    }
    SpecTcl::getInstance()->findWaveform("test2")->update(trace.data());

    // get the ramp:

    CPPUNIT_ASSERT_NO_THROW(m_pInterp->GlobalEval("spectcl::serial::waveform get test1 test2")); 

    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GetResultString();
    // Two waveforms:

    EQ(2, result.llength());
    CTCLObject wf1;
    wf1.Bind(*m_pInterp);
    wf1 = result.lindex(0);               // {test1 {upramp}}

    CTCLObject wf2;
    wf2.Bind(*m_pInterp);
    wf2 = result.lindex(1);             // {test2 {downramp}}

    // Analyze wf1:

    EQ(3, wf1.llength());
    EQ(std::string("test1"), std::string(wf1.lindex(0)));
    wf1 = wf1.lindex(1);
    EQ(100, wf1.llength());
    for (int i =0; i < 100; i++) {
        int value = wf1.lindex(i);
        EQ(i, value);
    }

    // Analyze wf2:

    EQ(3, wf2.llength());
    EQ(std::string("test2"), std::string(wf2.lindex(0)));
    wf2 = wf2.lindex(1);
    EQ(100, wf2.llength());
    for (int i =0; i < 100; i++) {
        int value = wf2.lindex(i);
        EQ(100 - i, value);
    }

}

void WFCmdTests::get_4() {
    // get without a waveform spec

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform get"),
        CTCLException
    );  
}

// metdata:

void
WFCmdTests::metadata_1() {
    // no subcommand:

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform metadata"),
        CTCLException
    );
}
void
WFCmdTests::metadata_2() {
    // Invalid subcommand:
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform metadata invalid"),
        CTCLException
    );
}

// metadata set:

void
WFCmdTests::mdset_1() {
    // Set a single bit of metadata:

    m_pInterp->GlobalEval("spectcl::serial::waveform create test 100");

    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform metadata set test a b")
    );

    // Did it set the metadata:

    auto wf = SpecTcl::getInstance()->findWaveform("test");
    std::string value;
    CPPUNIT_ASSERT_NO_THROW(value = wf->getMetadata("a"));
    EQ(std::string("b"), value);

}
void
WFCmdTests::mdset_2() {
    // Can set more than one metadata item per metadata set command.

    m_pInterp->GlobalEval("spectcl::serial::waveform create test 100");

    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform metadata set test a b c d")
    );
    auto wf = SpecTcl::getInstance()->findWaveform("test");
    std::string value;
    CPPUNIT_ASSERT_NO_THROW(value = wf->getMetadata("a"));
    EQ(std::string("b"), value);

    CPPUNIT_ASSERT_NO_THROW(value = wf->getMetadata("c"));
    EQ(std::string("d"), value);
}

void
WFCmdTests::mdset_3() {
    // no such waveform:

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform metadata set test a b"),
        CTCLException
    );
}
void
WFCmdTests::mdset_4() {
    // Too fiew parameters:

    m_pInterp->GlobalEval("spectcl::serial::waveform create test 100");
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::metadata set test"),
        CTCLException
    );
}
void
WFCmdTests::mdset_5() {
    // metadata name w/o value:

    m_pInterp->GlobalEval("spectcl::serial::waveform create test 100");
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::metadata set test mdname"),
        CTCLException
    );
}

// metadata get.

void
WFCmdTests::mdget_1() {
    // single item get.

    m_pInterp->GlobalEval("spectcl::serial::waveform create test 100");
    m_pInterp->GlobalEval("spectcl::serial::waveform metadata set test a b c d");

    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform metadata get test c")
    );

    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GetResultString();

    EQ(2, result.llength());     // name value pair.
    EQ(std::string("c"), std::string(result.lindex(0)));  // metadata name.
    EQ(std::string("d"), std::string(result.lindex(1)));  // value.
}

void 
WFCmdTests::mdget_2() {
    // Dump all metadata

    m_pInterp->GlobalEval("spectcl::serial::waveform create test 100");
    m_pInterp->GlobalEval("spectcl::serial::waveform metadata set test a b c d");

    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform metadata get test")
    );

    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GetResultString();

    EQ(4, result.llength());    // DIct with both items.

    // Will be in alpha order because (whitebox) it's a map.

    EQ(std::string("a"), std::string(result.lindex(0)));
    EQ(std::string("b"), std::string(result.lindex(1)));

    EQ(std::string("c"), std::string(result.lindex(2)));
    EQ(std::string("d"), std::string(result.lindex(3)));
}

void
WFCmdTests::mdget_3() {
    // Get no-such metadata

    m_pInterp->GlobalEval("spectcl::serial::waveform create test 100");
    m_pInterp->GlobalEval("spectcl::serial::waveform metadata set test a b c d");

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform metadata get george"),
        CTCLException
    );

}

void
WFCmdTests::mdget_4() {
    // Get from no such waveform

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::metadata get junk"),
        CTCLException
    );
}

// resize subcommand tests.

void
WFCmdTests::resize_1() {
    // good resize..

    m_pInterp->GlobalEval("spectcl::serial::waveform create test 100");
    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform resize test 200")
    );

    auto wf = SpecTcl::getInstance()->findWaveform("test");
    EQ(size_t(200), wf->size());
}
void
WFCmdTests::resize_2() {
    // bad integer 

    m_pInterp->GlobalEval("spectcl::serial::waveform create test 100");
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform resize test 10fa"),
        CTCLException
    );   
}

void
WFCmdTests::resize_3() {
    // no such waveform.

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform resize test 200"),
        CTCLException
    );
}
void
WFCmdTests::resize_4() {
    // Not enough command parameters.

    m_pInterp->GlobalEval("spectcl::serial::waveform create test 100");
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform resize test"),
        CTCLException
    );   
}

// Tests for fits (Issue #212)

void 
WFCmdTests::fits_1() {
    // Fits needs a waveform.
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform fits"),
        CTCLException
    );
}

void 
WFCmdTests::fits_2() {
    // there's a waveform, but too many parameters:

    CWaveform wf("test", 100);
    SpecTcl::getInstance()->addWaveform(wf);

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform fits test * extra-junk"),
        CTCLException
    );
}

void 
WFCmdTests::fits_3() {
    // Incorrect fit name:

    CWaveform wf("test", 100);
    SpecTcl::getInstance()->addWaveform(wf);

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform fits tests *"),
        CTCLException
    );
}

void 
WFCmdTests::fits_4() {
    std::string result;

    CWaveform wf("test", 100);
    SpecTcl::getInstance()->addWaveform(wf);

    CPPUNIT_ASSERT_NO_THROW(
       result = m_pInterp->GlobalEval("spectcl::serial::waveform fits test *")
    );

    CTCLObject lresult; 
    lresult.Bind(m_pInterp);
    lresult = std::string(result);

    EQ(0, lresult.llength());
}

void 
WFCmdTests::fits_5() {
    std::string result;

    CWaveform wf("test", 100);
    wf.addFit("Afit");
    SpecTcl::getInstance()->addWaveform(wf);

    CPPUNIT_ASSERT_NO_THROW(
        result = m_pInterp->GlobalEval("spectcl::serial::waveform fits test *")
    );

    CTCLObject lresult; 
    lresult.Bind(m_pInterp);
    lresult = std::string(result);
    EQ(1, lresult.llength());
}

void 
WFCmdTests::fits_6() {
    std::string result;

    CWaveform wf("test", 100);
    wf.addFit("Afit");
    SpecTcl::getInstance()->addWaveform(wf);

    CPPUNIT_ASSERT_NO_THROW(
        result = m_pInterp->GlobalEval("spectcl::serial::waveform fits test *")
    );

    CTCLObject lresult; 
    lresult.Bind(m_pInterp);
    lresult = std::string(result);
    
    // Simplify dict handling it's alist of name value name value....
    CTCLObject def; def.Bind(m_pInterp); def = lresult.lindex(0);  // The definition.
    EQ(4, def.llength());                                         // namekey name pointskey points
    EQ(std::string("name"), std::string(def.lindex(0)));           // Name key.    
    EQ(std::string("Afit"), std::string(def.lindex(1)));            // name value.
    EQ(std::string("points"), std::string(def.lindex(2)));       // points key.

    CTCLObject pts; pts.Bind(m_pInterp); pts = def.lindex(3);    // List of points.

    EQ(100, pts.llength());

    // All the points are "0.0"

    auto points = pts.getListElements();
    for (auto& p : points) {
        EQ(std::string("0.0"), std::string(p));
    }
}
void
WFCmdTests::fits_7() {
std::string result;

    CWaveform wf("test", 100);
    wf.addFit("Afit");
    SpecTcl::getInstance()->addWaveform(wf);

    CPPUNIT_ASSERT_NO_THROW(
        result = m_pInterp->GlobalEval("spectcl::serial::waveform fits test *")
    );

    CTCLObject lresult; 
    lresult.Bind(m_pInterp);
    lresult = std::string(result);
    
    EQ(1, lresult.llength());
}
// Tests for the getall subcommand.  They use the following utility:

std::pair<CWaveform*, CWaveform*> 
WFCmdTests::makeWfPair() {
    CWaveform wf1("a", 10);   // Firs is before the second.
    CWaveform wf2("b", 10);

    SpecTcl::getInstance()->addWaveform(wf1);
    SpecTcl::getInstance()->addWaveform(wf2);

    auto p1 = SpecTcl::getInstance()->findWaveform("a");
    auto p2 = SpecTcl::getInstance()->findWaveform("b");

    return {p1, p2};
}

void
WFCmdTests::getall_1() {
    /// Too few parameters, needs at least one waveform.

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform getall"),
        CTCLException
    );
}
void
WFCmdTests::getall_2() {
    makeWfPair();               // NO fits so I don't need the ptr.
    std::string result;
    CPPUNIT_ASSERT_NO_THROW(
        result = m_pInterp->GlobalEval("spectcl::serial::waveform getall a");
    );
    CTCLObject oResult; oResult.Bind(m_pInterp); oResult = result;

    // There should only be one element:
    EQ(1, oResult.llength());
    // It should describe "a" and have a 10 pt waveform:

    CTCLObject desc = oResult.lindex(0); desc.Bind(m_pInterp);
    std::string wfName = Tcl::DictGetAsStr(*m_pInterp, desc, "name");
    EQ(std::string("a"), wfName);

    CTCLObject waveform = Tcl::DictGet(*m_pInterp, desc, "waveform");   // waveform is bound.
    EQ(3, waveform.llength());
    EQ(std::string("a"), std::string(waveform.lindex(0)));
    CTCLObject trace = waveform.lindex(1); trace.Bind(m_pInterp);
    EQ(10, trace.llength());                 // Correct # of points.

    CTCLObject fits = Tcl::DictGet(*m_pInterp, desc, "fits");
    EQ(0, fits.llength());                         // No fits.


}
void
WFCmdTests::getall_3() {
    auto wfs = makeWfPair();
    // add a fit to the first one:

    wfs.first->addFit("afit");

    std::string result;
    CPPUNIT_ASSERT_NO_THROW(
        result = m_pInterp->GlobalEval("spectcl::serial::waveform getall a");
    );
    CTCLObject oResult; oResult.Bind(m_pInterp); oResult = result;

    // There should only be one element:
    EQ(1, oResult.llength());
    // It should describe "a" and have a 10 pt waveform:

    CTCLObject desc = oResult.lindex(0); desc.Bind(m_pInterp);
    std::string wfName = Tcl::DictGetAsStr(*m_pInterp, desc, "name");
    EQ(std::string("a"), wfName);

    CTCLObject waveform = Tcl::DictGet(*m_pInterp, desc, "waveform");   // waveform is bound.
    EQ(3, waveform.llength());
    EQ(std::string("a"), std::string(waveform.lindex(0)));
    CTCLObject trace = waveform.lindex(1); trace.Bind(m_pInterp);
    EQ(10, trace.llength());                 // Correct # of points.

    CTCLObject fits = Tcl::DictGet(*m_pInterp, desc, "fits");
    EQ(1, fits.llength());
    CTCLObject fit = fits.lindex(0);   // name "afit", points 10 element list.
    fit.Bind(m_pInterp);

    EQ(std::string("afit"), Tcl::DictGetAsStr(*m_pInterp, fit, "name"));
    
    CTCLObject points = Tcl::DictGet(*m_pInterp, fit, "points");
    points.Bind(m_pInterp);
    EQ(10, points.llength());
    
}
void
WFCmdTests::getall_4() {
    
}
void
WFCmdTests::getall_5() {
    
}
void
WFCmdTests::getall_6() {
    
}
void
WFCmdTests::getall_7() {
    
}
void
WFCmdTests::getall_8() {
    
}
void
WFCmdTests::getall_9() {
    
}