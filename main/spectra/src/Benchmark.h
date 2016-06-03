//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <iostream>

/*! \brief Simple benchmark class
 *
 * The benchmark class will time the elapsed microseconds between when it is constructed and destructed.
 * It relies on the automatic scoping of stack allocated objects, so do not create this with operator new.
 * It is used similar to the following:
 *
 * \code
 * { // start benchmarking region
 *   Benchmark<0, std::chrono::high_resolution_clock> benchmark;
 *
 *   // ... do some work that you want to benchmark
 *
 * } // end of scope to benchmark
 * \endcode
 */
template<int Msg, class Clock>
class Benchmark {
  std::chrono::time_point<Clock> begin;

  public:
  Benchmark(): begin(Clock::now()) {}
  ~Benchmark() {
    std::cerr << Msg << " duration : " << (Clock::now()-begin)/std::chrono::microseconds(1) << " us" << std::endl;
  }

};

#endif
