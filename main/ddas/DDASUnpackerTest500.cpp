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

// A test suite 
class DDASUnpacker500Test : public CppUnit::TestFixture
{
  private:
    DDASHit hit;

  public:
    CPPUNIT_TEST_SUITE( DDASUnpacker500Test );
    CPPUNIT_TEST( msps_0 );
    CPPUNIT_TEST( coarseTime_0 );
    CPPUNIT_TEST( time_0 );
    CPPUNIT_TEST( cfdFail_0 );
    CPPUNIT_TEST( cfdTrigSource_0 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {

      vector<uint32_t> data = { 0x0000000c, 0x0c0c01f4, 0x00084321, 
                                0x0000f687, 0x747f000a, 0x000008be};
      
      DDASHitUnpacker unpacker;
      tie(hit, ignore) = unpacker.unpack(data.data(), data.data()+data.size());
    }

    void tearDown() {

    }

    void msps_0 () {
      EQMSG("Simple body extracts adc frequency", uint32_t(500), hit.GetModMSPS()); 
    }

    void coarseTime_0 () {
      EQMSG("Simple body compute coarse time", uint64_t(0x000a0000f687)*10, hit.GetCoarseTime()); 
    }

    void time_0 () {
      ASSERTMSG("Simple body compute time",
                 std::abs(hit.GetTime() - 429497360715.281006)<0.000001); 
    }

    void cfdFail_0 () {
      EQMSG("Simple body computes cfd fail bit", uint32_t(0), hit.GetCFDFailBit());
    }

    void cfdTrigSource_0 () {
      EQMSG("Simple body compute cfd trig source bit", uint32_t(3), hit.GetCFDTrigSource());
    }
};

// Register it with the test factory
CPPUNIT_TEST_SUITE_REGISTRATION( DDASUnpacker500Test );
