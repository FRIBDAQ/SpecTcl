/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2016.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Jeromy Tompkins
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


#include <cppunit/extensions/HelperMacros.h>

#include "Asserts.h"
#include "DDASHit.h"
#include "DDASHitUnpacker.h"

#include <cstdint>
#include <cmath>
#include <vector>
#include <array>
#include <tuple>

using namespace std;
using namespace ::DAQ::DDAS;


// A test suite 
class DDASHitTest : public CppUnit::TestFixture
{
  private:
    DDASHit hit;

  public:
    CPPUNIT_TEST_SUITE( DDASHitTest );
    CPPUNIT_TEST( constructor_0 );
    CPPUNIT_TEST( reset_0 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {

      vector<uint32_t> data = { 0x0000002c, 0x0c0c0064, 0x00290321, 
                                0x0000f687, 0x947f000a, 0x000808be,
                                0x00000001, 0x00000002, 0x00000003,
                                0x00000004,
                                0x00000005, 0x00000006, 0x00000007,
                                0x00000008,
                                0x00000009, 0x0000000a, 0x0000000b,
                                0x0000000c,
                                0x00020001, 0x00040003, 0x00060005,
                                0x00080007};
      
      DDASHitUnpacker unpacker;
      unpacker.unpack(data.data(), data.data()+data.size(), hit);
    }

    void tearDown() {
    }

    void constructor_0() {
      DDASHit hit;
      testZeroInitialized(hit);
    }

    void testZeroInitialized(DDASHit& hit) 
    {
      // every data member should be zeroed out after a reset
      EQMSG("energy",   uint32_t(0), hit.GetEnergy());
      EQMSG("time low", uint32_t(0), hit.GetTimeLow());
      EQMSG("time high", uint32_t(0), hit.GetTimeHigh());
      EQMSG("time cfd", uint32_t(0), hit.GetTimeCFD());
      EQMSG("coarse time", uint64_t(0), hit.GetCoarseTime());
      EQMSG("computed time", 0., hit.GetTime());
      EQMSG("finish code", uint32_t(0), hit.GetFinishCode());
      EQMSG("channel length", uint32_t(0), hit.GetChannelLength());
      EQMSG("chan header length", uint32_t(0), hit.GetChannelLengthHeader());
      EQMSG("overflow code", uint32_t(0), hit.GetOverflowCode());
      EQMSG("slot id", uint32_t(0), hit.GetSlotID());
      EQMSG("crate id", uint32_t(0), hit.GetCrateID());
      EQMSG("channel id", uint32_t(0), hit.GetChannelID());
      EQMSG("mod msps", uint32_t(0), hit.GetModMSPS());
      EQMSG("cfd trig source", uint32_t(0), hit.GetCFDTrigSource());
      EQMSG("cfd fail bit", uint32_t(0), hit.GetCFDFailBit());
      EQMSG("trace length", uint32_t(0), hit.GetTraceLength());
      EQMSG("trace cleared", size_t(0), hit.GetTrace().size());
      EQMSG("energy sums cleared", size_t(0), hit.GetEnergySums().size());
      EQMSG("qdc sums cleared", size_t(0), hit.GetQDCSums().size());
    }

    void reset_0 () {
      hit.Reset();

      // every data member should be zeroed out after a reset
      EQMSG("energy",   uint32_t(0), hit.GetEnergy());
      EQMSG("time low", uint32_t(0), hit.GetTimeLow());
      EQMSG("time high", uint32_t(0), hit.GetTimeHigh());
      EQMSG("time cfd", uint32_t(0), hit.GetTimeCFD());
      EQMSG("coarse time", uint64_t(0), hit.GetCoarseTime());
      EQMSG("computed time", 0., hit.GetTime());
      EQMSG("finish code", uint32_t(0), hit.GetFinishCode());
      EQMSG("channel length", uint32_t(0), hit.GetChannelLength());
      EQMSG("chan header length", uint32_t(0), hit.GetChannelLengthHeader());
      EQMSG("overflow code", uint32_t(0), hit.GetOverflowCode());
      EQMSG("slot id", uint32_t(0), hit.GetSlotID());
      EQMSG("crate id", uint32_t(0), hit.GetCrateID());
      EQMSG("channel id", uint32_t(0), hit.GetChannelID());
      EQMSG("mod msps", uint32_t(0), hit.GetModMSPS());
      EQMSG("cfd trig source", uint32_t(0), hit.GetCFDTrigSource());
      EQMSG("cfd fail bit", uint32_t(0), hit.GetCFDFailBit());
      EQMSG("trace length", uint32_t(0), hit.GetTraceLength());
      EQMSG("trace cleared", size_t(0), hit.GetTrace().size());
      EQMSG("energy sums cleared", size_t(0), hit.GetEnergySums().size());
      EQMSG("qdc sums cleared", size_t(0), hit.GetQDCSums().size());
    }
};

// Register it with the test factory
CPPUNIT_TEST_SUITE_REGISTRATION( DDASHitTest );
