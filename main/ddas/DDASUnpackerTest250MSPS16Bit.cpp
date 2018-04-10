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
#include "DDASHitUnpacker.h"

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <vector>
#include <tuple>
#include <cmath>

using namespace std;
using namespace ::DAQ::DDAS;

// A test suite for testing the 250MSPS, 16Bit digitizer output.
// This must be tested separately from the older 250MSPS, 14Bit
// digitizers because the data formats differ slightly in the
// fourth data word.
class DDASUnpacker250MSPS16BitTest : public CppUnit::TestFixture
{
  private:
    DDASHit hit;

  public:
    CPPUNIT_TEST_SUITE( DDASUnpacker250MSPS16BitTest );
    CPPUNIT_TEST( msps_0 );
    CPPUNIT_TEST( rev_0 );
    CPPUNIT_TEST( resolution_0 );
    CPPUNIT_TEST( coarseTime_0 );
    CPPUNIT_TEST( time_0 );
    CPPUNIT_TEST( cfdFail_0 );
    CPPUNIT_TEST( cfdTrigSource_0 );
    CPPUNIT_TEST( traceLength_0 );
    CPPUNIT_TEST( trace_0 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {

        // define an event with no qdc or energy sums information but does include
        // a very short trace. The real distinction that we want to test is the
        // ability to extract a proper trace length and overflow/underflow bit.
      vector<uint32_t> data = { 0x0000000c, 0x0f1000fa, 0x000a4321,
                                0x0000f687, 0xf47f000a, 0x800208be,
                                0x45670123};
      
      DDASHitUnpacker unpacker;
      tie(hit, ignore) = unpacker.unpack(data.data(), data.data()+data.size());
    }

    void tearDown() {

    }

    void msps_0 () {
      EQMSG("Simple body extracts adc frequency", uint32_t(250), hit.GetModMSPS()); 
    }

    void rev_0 () {
      EQMSG("Simple body extracts hardware revision", 15, hit.GetHardwareRevision()); 
    }

    void resolution_0 () {
      EQMSG("Simple body extracts adc resolution", 16, hit.GetADCResolution()); 
    }

    void coarseTime_0 () {
      EQMSG("Simple body compute coarse time", uint64_t(0x000a0000f687)*8, hit.GetCoarseTime()); 
    }

    void time_0 () {
      ASSERTMSG("Simple body compute time",
                 std::abs(hit.GetTime() - 343597888567.2810059)<0.000001); 
    }

    void cfdFail_0 () {
      EQMSG("Simple body compute cfd fail bit", uint32_t(1), hit.GetCFDFailBit());
    }

    void cfdTrigSource_0 () {
      EQMSG("Simple body compute cfd trig source bit", uint32_t(1), hit.GetCFDTrigSource());
    }

    void traceLength_0 () {
        EQMSG("bit 31 does not get included in trace length", uint32_t(2), hit.GetTraceLength());
    }

    void trace_0() {
        ASSERTMSG("trace content", std::vector<uint16_t>({0x0123, 0x4567}) == hit.GetTrace());
    }

    void overflowUnderflow_0 () {
        EQMSG("bit 31 is the overflow underflow bit", true, hit.GetADCOverflowUnderflow());
    }
};

// Register it with the test factory
CPPUNIT_TEST_SUITE_REGISTRATION( DDASUnpacker250MSPS16BitTest );
