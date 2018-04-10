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


template<class T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vec)
{
  stream << "{ ";
  for (auto& element : vec ) stream << element << " ";
  stream << "}";

  return stream;
}

template<class T, long unsigned int N>
std::ostream& operator<<(std::ostream& stream, const std::array<T,N>& vec)
{
  stream << "{ ";
  for (int i=0; i<N; ++i) stream << vec[i] << " ";
  stream << "}";

  return stream;
}

// A test suite 
class DDASUnpackerTest : public CppUnit::TestFixture
{
  private:
    DDASHit hit;

  public:
    CPPUNIT_TEST_SUITE( DDASUnpackerTest );
    CPPUNIT_TEST( crateId_0 );
    CPPUNIT_TEST( slotId_0 );
    CPPUNIT_TEST( chanId_0 );
    CPPUNIT_TEST( headerLength_0 );
    CPPUNIT_TEST( eventLength_0 );
    CPPUNIT_TEST( finishCode_0 );
    CPPUNIT_TEST( msps_0 );
    CPPUNIT_TEST( timelow_0 );
    CPPUNIT_TEST( timehigh_0 );
    CPPUNIT_TEST( coarseTime_0 );
    CPPUNIT_TEST( time_0 );
    CPPUNIT_TEST( cfdFail_0 );
    CPPUNIT_TEST( cfdTrigSource_0 );
    CPPUNIT_TEST( energySums_0 );
    CPPUNIT_TEST( qdcSums_0 );
    CPPUNIT_TEST( trace_0 );
    CPPUNIT_TEST( externalClock_0);
    CPPUNIT_TEST( externalClock_1);
    CPPUNIT_TEST( externalClock_2);
    CPPUNIT_TEST( externalClock_3);
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {

      vector<uint32_t> data = { 0x0000002c, 0x0c0c0064, 0x002d2321, 
                                0x0000f687, 0x947f000a, 0x000808be,
                                0x00000001, 0x00000002, 0x00000003,
                                0x00000004,
                                0x00000005, 0x00000006, 0x00000007,
                                0x00000008,
                                0x00000009, 0x0000000a, 0x0000000b,
                                0x0000000c,
                                0x0a0a0b0b, 0x0c0c0d0d,
                                0x00020001, 0x00040003, 0x00060005,
                                0x00080007};
      
      DDASHitUnpacker unpacker;
      tie(hit, ignore) = unpacker.unpack(data.data(), data.data()+data.size());
    }

    void tearDown() {

    }

    void crateId_0 () {
      EQMSG("Simple body extracts crate id", uint32_t(3), hit.GetCrateID()); 
    }
    void slotId_0 () {
      EQMSG("Simple body extracts slot id", uint32_t(2), hit.GetSlotID());
    }
    void chanId_0 () {
      EQMSG("Simple body extracts channel id", uint32_t(1), hit.GetChannelID()); 
    }
    void headerLength_0 () {
      EQMSG("Simple body extracts header length", uint32_t(18), hit.GetChannelLengthHeader()); 
    }
    void eventLength_0 () {
      EQMSG("Simple body extracts event length", uint32_t(22), hit.GetChannelLength()); 
    }

    void finishCode_0 () {
      EQMSG("Simple body extracts finish code", uint32_t(0), hit.GetFinishCode()); 
    }

    void msps_0 () {
      EQMSG("Simple body extracts adc frequency", uint32_t(100), hit.GetModMSPS()); 
    }

    void timelow_0 () {
      EQMSG("Simple body extracts time low", uint32_t(63111), hit.GetTimeLow()); 
    }
    void timehigh_0 () {
      EQMSG("Simple body extracts time high", uint32_t(10), hit.GetTimeHigh()); 
    }
    void coarseTime_0 () {
      EQMSG("Simple body coarse time", uint64_t(0x000a0000f687)*10, hit.GetCoarseTime()); 
    }
    void time_0 () {
      ASSERTMSG("Simple body full time",
                 std::abs(hit.GetTime()-429497360711.601257) < 0.000001);
    }

    void cfdFail_0 () {
      EQMSG("Simple body compute cfd fail bit", uint32_t(1), hit.GetCFDFailBit());
    }

    void cfdTrigSource_0 () {
      EQMSG("Simple body compute cfd trig source bit", uint32_t(0), hit.GetCFDTrigSource());
    }

    void energySums_0 () {
      std::vector<uint32_t> expected = {1, 2, 3, 4};
      EQMSG("Found all 4 energy sums",
            expected, hit.GetEnergySums());
    }

    void qdcSums_0 () {
      std::vector<uint32_t> expected = {5,6,7,8,9,10,11,12};
      EQMSG("Found all 4 energy sums",
            expected, hit.GetQDCSums());
    }

    void trace_0 () {
      std::vector<uint16_t> expected = {1, 2, 3, 4, 5, 6, 7, 8};
      EQMSG("Found all trace samples",
            expected, hit.GetTrace());
    }

    void externalClock_0() {
      uint64_t expected = 0xc0c0d0d0a0a0b0b;
      EQMSG("Timestamp found with QDC and energy enabled",
            expected, hit.GetExternalTimestamp());
    }

  void externalClock_1() {
    // No QDC or energy sums, external timestamp still present... header length = 6
    std::vector<uint32_t> data = { 0x0000000e, 0x0c0c0064, 
      0x000c6000, 0x139f2709, 0x28170000,
      0x00007fff, 0x40302010, 0x00a00a00 };

    DDASHit hit;
    DDASHitUnpacker unpacker;
    unpacker.unpack(data.data(), data.data()+data.size(), hit);
    
    EQMSG("Timestamp extracted when no qdc or energy sum",
       uint64_t(0x00a00a0040302010), hit.GetExternalTimestamp());
  }

  void externalClock_2() {
    // Energy sums/baselines present with external timestamp... header length = 10
    std::vector<uint32_t> data = { 0x0000000e, 0x0c0c0064, 
      0x0014a000, 0x139f2709, 0x28170000,
      0x00007fff, 0x00000001, 0x00000002, 
      0x00000003, 0x00000004, 
      0x40302010, 0x00a00a00 };

    DDASHit hit;
    DDASHitUnpacker unpacker;
    unpacker.unpack(data.data(), data.data()+data.size(), hit);
    
    EQMSG("Timestamp extracted when no qdc",
       uint64_t(0x00a00a0040302010), hit.GetExternalTimestamp());
  }

  void externalClock_3() {
    // QDC present with external timestamp... header length = 14
    std::vector<uint32_t> data = { 0x0000000e, 0x0c0c0064, 
      0x001ce000, 0x139f2709, 0x28170000,
      0x00007fff, 0x00000001, 0x00000002, 
      0x00000003, 0x00000004, 0x00000005,
      0x00000006, 0x00000007, 0x00000008,
      0x40302010, 0x00a00a00 };

    DDASHit hit;
    DDASHitUnpacker unpacker;
    unpacker.unpack(data.data(), data.data()+data.size(), hit);
    
    EQMSG("Timestamp extracted when no energy sum",
       uint64_t(0x00a00a0040302010), hit.GetExternalTimestamp());
}
};

// Register it with the test factory
CPPUNIT_TEST_SUITE_REGISTRATION( DDASUnpackerTest );
