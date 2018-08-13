// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>

#include <TreeParameter.h>
#include <Event.h>
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
  CPPUNIT_TEST_SUITE_END();


private:
    std::string m_fileName;
public:
  void setUp() {
    m_fileName = "";
  }
  void tearDown() {
    if(!m_fileName.empty()) unlink(m_fileName.c_str());
  }
protected:
  void construction();
  void oneCrateOneModule();
  void oneCrateTwoModules();
  void twoCrates();
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