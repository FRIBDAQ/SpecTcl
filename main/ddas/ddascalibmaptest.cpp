// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <Asserts.h>

#define private public
#include "CCalibratedFileDrivenParameterMapper.h"
#undef private

#include <Histogrammer.h>
#include <Event.h>
#include <TreeParameter.h>
#include <Globals.h>
#include <DDASHit.h>
#include <TCLInterpreter.h>

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>

class Calibunpacktests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(Calibunpacktests);
  CPPUNIT_TEST(emptyconfig);             // Configuration file processing tests
  CPPUNIT_TEST(onelineconstcalibrations);
  CPPUNIT_TEST(onelinevarcalibs);
  CPPUNIT_TEST(multilinemixed);
  CPPUNIT_TEST(missingFile);
  CPPUNIT_TEST(badLine);
  CPPUNIT_TEST(duplicateSpec);
  
  CPPUNIT_TEST(onehitOk);              // Unpacking tests
  CPPUNIT_TEST(twohitsOk);
  CPPUNIT_TEST(missingMapError);
  
  CPPUNIT_TEST_SUITE_END();


private:
    std::string m_fileName;
    CHistogrammer* m_pHistogrammer;
    CEvent*     m_pEvent;
    CTCLInterpreter* m_pInterp;
public:
  void setUp() {
    m_fileName = "";
    
    m_pHistogrammer = new CHistogrammer;
    gpEventSink     = m_pHistogrammer;
    
    m_pEvent = new CEvent;
    CTreeParameter::setEvent(*m_pEvent);
    
    m_pInterp = new CTCLInterpreter();
    gpInterpreter = m_pInterp;
  }
  void tearDown() {
    if(!m_fileName.empty()) unlink(m_fileName.c_str());
    delete m_pEvent;
    delete m_pHistogrammer;
    delete m_pInterp;
}
protected:
  void emptyconfig();
  void onelineconstcalibrations();
  void onelinevarcalibs();
  void multilinemixed();
  void missingFile();
  void badLine();
  void duplicateSpec();

  void onehitOk();
  void twohitsOk();
  void missingMapError();
private:
    void makeConfigFile(const std::vector<const char*>& cfg);
    DAQ::DDAS::DDASHit makeHit(unsigned c, unsigned s, unsigned a, double e);
};

CPPUNIT_TEST_SUITE_REGISTRATION(Calibunpacktests);

// Utilities

//
// Make a config file.
// cfg is a vector of lines to write the file.
void Calibunpacktests::makeConfigFile(const std::vector<const char*>& cfg)
{
    m_fileName = tmpnam(nullptr);
    std::ofstream c(m_fileName);
    for (auto i = 0; i < cfg.size(); i++) {
        c << cfg[i] << std::endl;
    }
}

// Make a hit given crate, slot, channel, energy

DAQ::DDAS::DDASHit
Calibunpacktests::makeHit(unsigned c, unsigned s, unsigned a, double e)
{
    DAQ::DDAS::DDASHit result;
    result.setCrate(c);
    result.setSlot(s);
    result.setChannel(a);
    result.setEnergy(e);
    
    return result;
}
// Empty configuration leads to no exceptions and an empty map.

void Calibunpacktests::emptyconfig() {
    CCalibratedFileDrivenParameterMapper* pMapper;
    CPPUNIT_ASSERT_NO_THROW(
        pMapper = new CCalibratedFileDrivenParameterMapper("/dev/null")
    );
    
    EQ(size_t(0), pMapper->m_map.size());
    EQ(std::string("/dev/null"), pMapper->m_configFile);
    
    delete pMapper;
}

// Test configuration of a single line with constant values for calibrations.

void Calibunpacktests::onelineconstcalibrations()
{
    CCalibratedFileDrivenParameterMapper* pMapper;
    
    std::vector<const char*> cfg = {
        "0 2 0 12 crate0.slot2.raw.chan0 0 0 0 0 100 100 MeV crate0.slot2.cal.chan0"       
    };
    makeConfigFile(cfg);
    
    CPPUNIT_ASSERT_NO_THROW(
        pMapper = new CCalibratedFileDrivenParameterMapper(m_fileName.c_str())
    );
    
    // Should have a non-null map we can drill down into:
    
    EQ(size_t(1), pMapper->m_map.size());
    EQ(size_t(3), pMapper->m_map[0].size());    //  Slot map goes to slot 2.
    EQ(size_t(1), pMapper->m_map[0][2].size());  // Channel 0 in slot2 makes one entry.
    ASSERT(pMapper->m_map[0][2][0]);             // We've got a record for this channel.
    
    // Now the contents of the channel:
    
   CCalibratedFileDrivenParameterMapper::pParameterInfo pInfo =
    pMapper->m_map[0][2][0];
    
    // Crate slot channel should match:
    
    EQ(unsigned(0), pInfo->s_crate);
    EQ(unsigned(2), pInfo->s_slot);
    EQ(unsigned(0), pInfo->s_channel);
    
    // Raw and calibrated parameters have the right names:
    
    EQ(std::string("crate0.slot2.raw.chan0"), pInfo->s_raw->getName());
    EQ(std::string("crate0.slot2.cal.chan0"), pInfo->s_calibrated->getName());
    
    // Calibrations are constant 0's.
    
    CTreeVariable* pNull(nullptr);
    EQ(double(0.0), pInfo->s_calibration.s_const.s_const);  // Const
    EQ(pNull, pInfo->s_calibration.s_const.s_pVariable);
    
    EQ(double(0.0), pInfo->s_calibration.s_linear.s_const);  // Const
    EQ(pNull, pInfo->s_calibration.s_linear.s_pVariable);
    
    EQ(double(0.0), pInfo->s_calibration.s_quadratic.s_const);  // Const
    EQ(pNull, pInfo->s_calibration.s_quadratic.s_pVariable);
    
    delete pMapper;
}


// Calibrations with variables specified.

void Calibunpacktests::onelinevarcalibs()
{
    CCalibratedFileDrivenParameterMapper* pMapper;
    
    std::vector<const char*> cfg = {
        "0 2 0 12 crate0.slot2.raw.chan0  const lin quad 0 100 100 MeV crate0.slot2.cal.chan0"

    };
    makeConfigFile(cfg);
    
    CPPUNIT_ASSERT_NO_THROW(
        pMapper = new CCalibratedFileDrivenParameterMapper(m_fileName.c_str())
    );
    
    CCalibratedFileDrivenParameterMapper::pParameterInfo pInfo =
        pMapper->m_map[0][2][0];
    
    
    ASSERT(pInfo->s_calibration.s_const.s_pVariable);
    EQ(std::string("const"), pInfo->s_calibration.s_const.s_pVariable->getName());
    
    ASSERT(pInfo->s_calibration.s_linear.s_pVariable);
    EQ(std::string("lin"), pInfo->s_calibration.s_linear.s_pVariable->getName());
    
    ASSERT(pInfo->s_calibration.s_quadratic.s_pVariable);
    EQ(std::string("quad"), pInfo->s_calibration.s_quadratic.s_pVariable->getName());
    
    delete pMapper;
        
}

// One const calibrationm, one linear calibration.

void Calibunpacktests::multilinemixed()
{
    std::vector<const char*> cfg = {
        "0 2 0 12 crate0.slot2.raw.chan0  const lin quad 0 100 100 MeV crate0.slot2.cal.chan0",
        "1 3 1 12 crate1.slot3.raw.chan1 0 0 0 0 100 100 MeV crate1.slot3.cal.chan1"       

    };
    makeConfigFile(cfg);
    CCalibratedFileDrivenParameterMapper* pMapper;
    CPPUNIT_ASSERT_NO_THROW(
        pMapper = new CCalibratedFileDrivenParameterMapper(m_fileName.c_str())
    );
    
    // 0,2,0 has info,
    // 0,3,0 does not
    // 0,3,1 does however:
    
    ASSERT(pMapper->m_map[0][2][0]);
    ASSERT(!pMapper->m_map[1][3][0]);
    ASSERT(pMapper->m_map[1][3][1]);
    
    // The assumption is that if we get the right one to be const and the
    // right one to be variable we get the right names given our previous tests.
    
    
    CCalibratedFileDrivenParameterMapper::pParameterInfo pVar =
        pMapper->m_map[0][2][0];
    CCalibratedFileDrivenParameterMapper::pParameterInfo pConst =
        pMapper->m_map[1][3][1];
        
        
    ASSERT(pVar->s_calibration.s_const.s_pVariable);
    ASSERT(pVar->s_calibration.s_linear.s_pVariable);
    ASSERT(pVar->s_calibration.s_quadratic.s_pVariable);
    
    ASSERT(!pConst->s_calibration.s_const.s_pVariable);
    ASSERT(!pConst->s_calibration.s_linear.s_pVariable);
    ASSERT(!pConst->s_calibration.s_quadratic.s_pVariable);
    
    delete pMapper;
}
// Constructing on a nonexistent file is an error.

void Calibunpacktests::missingFile()
{
    const char* file = tmpnam(nullptr);   // Get a name but don't create...
    
    CPPUNIT_ASSERT_THROW(
        CCalibratedFileDrivenParameterMapper* p =
            new CCalibratedFileDrivenParameterMapper(file),
            std::string
    );
}

// Parse fail on a line:

void Calibunpacktests::badLine()
{
    std::vector<const char*> cfg = {
        "0 2 0 12 crate0.slot2.raw.chan0  const lin quad 0 100 100 MeV crate0.slot2.cal.chan0",
        "1 3 george 12 crate0.slot2.raw.chan0 0 0 0 0 100 100 MeV crate0.slot2.cal.chan0"       

    };
    makeConfigFile(cfg);
    
    CPPUNIT_ASSERT_THROW(
        CCalibratedFileDrivenParameterMapper* p =
            new CCalibratedFileDrivenParameterMapper(m_fileName.c_str()),
            std::string
    );
}

// Duplicate channel spec.

void Calibunpacktests::duplicateSpec()
{
    std::vector<const char*> cfg = {
        "0 2 0 12 crate0.slot2.raw.chan0  const lin quad 0 100 100 MeV crate0.slot2.cal.chan0",
        "0 2 0 12 crate0.slot2.raw.chan0  const lin quad 0 100 100 MeV crate0.slot2.cal.chan0"

    };
    makeConfigFile(cfg);
    
    CPPUNIT_ASSERT_THROW(
        CCalibratedFileDrivenParameterMapper* p =
            new CCalibratedFileDrivenParameterMapper(m_fileName.c_str()),
            std::string
    );
    
}

// Event with a single hit all ok.

void Calibunpacktests::onehitOk()
{
    std::vector<const char*> cfg = {
       "0 2 0 12 crate0.slot2.raw.chan0  0 2 0 0 100 100 MeV crate0.slot2.cal.chan0"
    };
    makeConfigFile(cfg);
    CCalibratedFileDrivenParameterMapper m (m_fileName.c_str());
    CTreeParameter::BindParameters();
    
    std::vector<DAQ::DDAS::DDASHit> hits;
    hits.push_back(makeHit(0, 2, 0, 100.0));
    
    m.mapToParameters(hits, *m_pEvent);
    
    // The raw parameter for 0,2,12 should be set and 100.00
    //  the calibrated one should be set and 200.
    
    CCalibratedFileDrivenParameterMapper::pParameterInfo pInfo =
         m.m_map[0][2][0];
    
    ASSERT(pInfo->s_raw->isValid());
    ASSERT(pInfo->s_calibrated->isValid());
    
    double raw = *(pInfo->s_raw);
    double cal = *(pInfo->s_calibrated);
    
    EQ(100.0, raw);
    EQ(200.0, cal);

}

// Events with two hits that are both ok.  Note one is calibrated with consts,
// the other is calibrated with vars.
//

void Calibunpacktests::twohitsOk()
{
    std::vector<const char*> cfg = {
        "0 2 0 12 crate0.slot2.raw.chan0  const lin quad 0 100 100 MeV crate0.slot2.cal.chan0",
        "1 3 1 12 crate1.slot3.raw.chan1 0 1 0 0 100 100 MeV crate1.slot3.cal.chan1"       

    };
    makeConfigFile(cfg);
    
    // Set the calibration variables:
    
    CTreeVariable c("const", 0.0, "");
    CTreeVariable l("lin", 1.5, "");
    CTreeVariable q("quad", 0.0, "");
    

    // make the hits.
    
    std::vector<DAQ::DDAS::DDASHit> hits;
    hits.push_back(makeHit(0, 2, 0, 100.0));
    hits.push_back(makeHit(1, 3, 1, 150.0));
    
    // Make the mapper and bind the parameters
    
    CCalibratedFileDrivenParameterMapper m(m_fileName.c_str());
    CTreeParameter::BindParameters;
    CTreeVariable::BindVariables(*m_pInterp);
    
    c = 0.0;
    l = 1.5;         // Constructed value does nothing :-(
    q = 0.0;
    
    m.mapToParameters(hits, *m_pEvent);
    
    // 0,2,0:
    
    CCalibratedFileDrivenParameterMapper::pParameterInfo pInfo =
         m.m_map[0][2][0];
    
    ASSERT(pInfo->s_raw->isValid());
    ASSERT(pInfo->s_calibrated->isValid());
    
    double raw = *(pInfo->s_raw);
    double cal = *(pInfo->s_calibrated);
    
    EQ(100.0, raw);
    EQ(150.0, cal);
    
    // 1,3,1:
    
    pInfo = m.m_map[1][3][1];
    ASSERT(pInfo->s_raw->isValid());
    ASSERT(pInfo->s_calibrated->isValid());
    
    raw = *(pInfo->s_raw);
    cal = *(pInfo->s_calibrated);
    
    EQ(150.0, raw);
    EQ(150.0, cal);


}

// Processing an event with an unmapped hit is an error.

void Calibunpacktests::missingMapError()
{
    std::vector<const char*> cfg = {
        "0 2 0 12 crate0.slot2.raw.chan0  const lin quad 0 100 100 MeV crate0.slot2.cal.chan0",
        "1 3 1 12 crate1.slot3.raw.chan1 0 1 0 0 100 100 MeV crate1.slot3.cal.chan1"       

    };
    makeConfigFile(cfg);
    
    // Set the calibration variables:
    
    CTreeVariable c("const", 0.0, "");
    CTreeVariable l("lin", 1.5, "");
    CTreeVariable q("quad", 0.0, "");
    

    // make the hits.
    
    std::vector<DAQ::DDAS::DDASHit> hits;
    hits.push_back(makeHit(0, 2, 0, 100.0));
    hits.push_back(makeHit(0, 2, 1, 123.0));   // No map for this.
    hits.push_back(makeHit(1, 3, 1, 150.0));
    
    // Make the mapper and bind the parameters
    
    CCalibratedFileDrivenParameterMapper m(m_fileName.c_str());
    CTreeParameter::BindParameters;
    CTreeVariable::BindVariables(*m_pInterp);
    
    c = 0.0;
    l = 1.5;         // Constructed value does nothing :-(
    q = 0.0;
    
    
    CPPUNIT_ASSERT_THROW(
      m.mapToParameters(hits, *m_pEvent),
      std::string
    );
      
}