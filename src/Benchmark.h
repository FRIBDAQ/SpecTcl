
#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <iostream>

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
