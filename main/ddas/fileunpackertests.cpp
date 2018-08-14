// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>

#include <TreeParameter.h>
#include <Event.h>
#include <Histogrammer.h>
#include <Globals.h>

#include "DDASHit.h"
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <fstream>

#define private public
#include "CFileDrivenParameterMapper.h"
#undef public

class fileupktests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(fileupktests);
  CPPUNIT_TEST(construction);
  CPPUNIT_TEST(oneCrateOneModule);
  CPPUNIT_TEST(oneCrateTwoModules);
  CPPUNIT_TEST(twoCrates);
  
  CPPUNIT_TEST(badParse);
  CPPUNIT_TEST(duplicateAddress);
  
  CPPUNIT_TEST(singleGoodHit);
  CPPUNIT_TEST(multiGoodHits);
  
  CPPUNIT_TEST(unmappedHit);
  CPPUNIT_TEST_SUITE_END();


private:
    std::string m_fileName;
    CHistogrammer* m_pHistogrammer;
    CEvent*     m_pEvent;
public:
  void setUp() {
    m_fileName = "";
    
    m_pHistogrammer = new CHistogrammer;
    gpEventSink     = m_pHistogrammer;
    
    m_pEvent = new CEvent;
    CTreeParameter::setEvent(*m_pEvent);
    
  }
  void tearDown() {
    if(!m_fileName.empty()) unlink(m_fileName.c_str());
    delete m_pEvent;
    delete m_pHistogrammer;
  }
protected:
  void construction();
  
  // Good config files.
  
  void oneCrateOneModule();
  void oneCrateTwoModules();
  void twoCrates();
  
  // Bad config files.
  
  void badParse();
  void duplicateAddress();
  
  // Good unpacks.
  
  void singleGoodHit();
  void multiGoodHits();
  
  // Failed unpacks.
  
  void unmappedHit();
  
};


/*
 * createConfigFile
 *   Yeah I know, using tempnam isn't multiprocess or thread safe.. .but I
 *   need a file somewhere that I can write to here and read as a configfile later
 *   on.
 * @param contents - string vector of contents.  Each string is written terminated
 *                   by std::endl;
 * @return std::string -name of the file created and populated.
 */
static std::string
createConfigFile(const std::vector<std::string>& contents)
{
    std::string fileName(tempnam(nullptr, "cfg"));
    std::ofstream c(fileName.c_str());
    
    for (int i =0; i < contents.size(); i++) {
        c << contents[i] << std::endl;
    }
    
    return fileName;
}

/**
 * makeHit
 *    Make a (minimal) hit:
 *
 *  @param crate - crate of the hit.
 *  @param slot  - Slot of the hit
 *  @param channel - Channel of hit.
 *  @param energy  - Energy of the hit
 *  @return DAQ::DDAS::DDASHit - the resulting hit.l
 */
static DAQ::DDAS::DDASHit
makeHit(unsigned crate, unsigned slot, unsigned channel, unsigned energy)
{
    DAQ::DDAS::DDASHit hit;
    hit.setCrate(crate);
    hit.setSlot(slot);
    hit.setChannel(channel);
    hit.setEnergy(energy);
    

    return hit;    
}
/**
 * verifyParamerter
 *    Verify that a parameter has a value and that it is specific.
 *
 * @param name - name of the parameter.
 * @param value - required value of the parameter.
 */
static void verifyParameter(const char* name, double value)
{
    CTreeParameter param(name);
    param.Bind();
    ASSERT(param.isValid());
    double pvalue = param;
    EQ(value, pvalue);
}

CPPUNIT_TEST_SUITE_REGISTRATION(fileupktests);

void fileupktests::construction() {
    CPPUNIT_ASSERT_NO_THROW(CFileDrivenParameterMapper nullMapper("/dev/null"));
    
    // Map should be empty:
    
    CFileDrivenParameterMapper nullMapper("/dev/null");
    EQ(std::string("/dev/null"), nullMapper.m_configFile);
    ASSERT(nullMapper.m_map.empty());
}


void fileupktests::oneCrateOneModule()
{
    // Config file with a single module:
    
    std::vector<std::string> configFile = {
        "0 2 0 12 slot2.channel0",
        "0 2 1 12 slot2.channel1",
        "0 2 2 12 slot2.channel2"
    };
    std::string file = createConfigFile(configFile);
    m_fileName = file;
    CFileDrivenParameterMapper mapper(file.c_str());
 
    // The map should be correct:  one crate, three slots (number from 0), 3 channels.
    // with the right values:
    
    CFileDrivenParameterMapper::CrateMap& map(mapper.m_map);
    
    EQ(size_t(1), map.size());
    EQ(size_t(3), map[0].size());
    EQ(size_t(3), map[0][2].size());
    
    EQ(std::string("slot2.channel0"), map[0][2][0]->getName());
    EQ(std::string("slot2.channel1"), map[0][2][1]->getName());
    EQ(std::string("slot2.channel2"), map[0][2][2]->getName());
    
    
}

void fileupktests::oneCrateTwoModules()
{
    std::vector<std::string> configFile = {
        "0 2 0 12 slot2.channel0",
        "0 2 1 12 slot2.channel1",
        "0 3 0 14 slot3.channel0"
    };
    m_fileName = createConfigFile(configFile);
    
    CFileDrivenParameterMapper mapper(m_fileName.c_str());
    CFileDrivenParameterMapper::CrateMap& map(mapper.m_map);
    
    EQ(size_t(4), map[0].size());    // Slots 2,3.
    
    EQ(std::string("slot3.channel0"), map[0][3][0]->getName());
}

void fileupktests::twoCrates()
{
    std::vector<std::string> configFile = {
        "0 2 0 12 slot2.channel0",
        "0 2 1 12 slot2.channel1",
        "0 3 0 14 slot3.channel0",
        "1 2 0 12 crate1.slot2.channel0",
        "2 2 0 12 crate2.slot2.channel0"
    };
    m_fileName = createConfigFile(configFile);
    
    CFileDrivenParameterMapper mapper(m_fileName.c_str());
    CFileDrivenParameterMapper::CrateMap& map(mapper.m_map);
    
    EQ(size_t(3), map.size());
    
    EQ(std::string("crate1.slot2.channel0"), map[1][2][0]->getName());
    EQ(std::string("crate2.slot2.channel0"), map[2][2][0]->getName());
}

void fileupktests::badParse()
{
        std::vector<std::string> configFile = {
        "0 2 0 12 slot2.channel0",
        "larry curly moe shemp george"           // Ilegal.
        };
        m_fileName = createConfigFile(configFile);
        
        CPPUNIT_ASSERT_THROW(
            CFileDrivenParameterMapper mapper(m_fileName.c_str()),
            std::string
        );
}

void fileupktests::duplicateAddress()
{
    std::vector<std::string> configFile = {
        "0 2 0 12 slot2.channel0",
        "0 2 0 12 slot2.channel0"
    };
    m_fileName = createConfigFile(configFile);
    
    CPPUNIT_ASSERT_THROW(
        CFileDrivenParameterMapper mapper(m_fileName.c_str()),
        std::string
    );
}


void fileupktests::singleGoodHit()
{
    std::vector<std::string> configFile = {
        "0 2 0 12 slot2.channel0",
        "0 2 1 12 slot2.channel1",
        "0 3 0 14 slot3.channel0",
        "1 2 0 12 crate1.slot2.channel0",
        "2 2 0 12 crate2.slot2.channel0"
    };
    
    m_fileName = createConfigFile(configFile);
    CFileDrivenParameterMapper mapper(m_fileName.c_str());
    CTreeParameter::BindParameters();
    
    // Create a vector of single hits:
    
    std::vector<DAQ::DDAS::DDASHit> hits;
    hits.push_back(makeHit(0,2,1,1234));
    
    // Unpack the data and check that it wound up in the parameter named
    // "slot2.channel1"
    
    mapper.mapToParameters(hits, *m_pEvent);
    verifyParameter("slot2.channel1", 1234);
}


void fileupktests::multiGoodHits()
{
    std::vector<std::string> configFile = {
        "0 2 0 12 slot2.channel0",
        "0 2 1 12 slot2.channel1",
        "0 3 0 14 slot3.channel0",
        "1 2 0 12 crate1.slot2.channel0",
        "2 2 0 12 crate2.slot2.channel1"
    };
    
    m_fileName = createConfigFile(configFile);
    CFileDrivenParameterMapper mapper(m_fileName.c_str());
    CTreeParameter::BindParameters();
 
    std::vector<DAQ::DDAS::DDASHit> hits;
    hits.push_back(makeHit(0, 2, 0, 100));
    hits.push_back(makeHit(0, 2, 1, 200));
    hits.push_back(makeHit(0, 3, 0, 300));
    hits.push_back(makeHit(1, 2, 0, 400));
    hits.push_back(makeHit(2, 2, 0, 500));
    
    mapper.mapToParameters(hits, *m_pEvent);
    verifyParameter("slot2.channel0", 100);
    verifyParameter("slot2.channel1", 200);
    verifyParameter("slot3.channel0", 300);
    verifyParameter("crate1.slot2.channel0", 400);
    verifyParameter("crate2.slot2.channel1", 500);
}

void fileupktests::unmappedHit()
{
    std::vector<std::string> configFile = {
        "0 2 0 12 slot2.channel0",
        "0 2 1 12 slot2.channel1",
        "0 3 0 14 slot3.channel0",
        "1 2 0 12 crate1.slot2.channel0",
        "2 2 0 12 crate2.slot2.channel1"
    };
    
    m_fileName = createConfigFile(configFile);
    CFileDrivenParameterMapper mapper(m_fileName.c_str());
    CTreeParameter::BindParameters();
 
    std::vector<DAQ::DDAS::DDASHit> hits;
    hits.push_back(makeHit(0, 2, 0, 100));
    hits.push_back(makeHit(0, 2, 1, 200));
    hits.push_back(makeHit(0, 3, 0, 300));
    hits.push_back(makeHit(1, 2, 0, 400));
    hits.push_back(makeHit(3, 2, 0, 500));    // No mapping for this hit.
    
    CPPUNIT_ASSERT_THROW(
        mapper.mapToParameters(hits, *m_pEvent),
        std::string
    );
}