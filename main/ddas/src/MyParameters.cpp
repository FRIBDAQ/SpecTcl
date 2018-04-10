
#include "MyParameters.h"

using namespace std;

////
//
// 
void ChannelData::Initialize(string name) {
  // energy : 4096 channels between 0 and 4096
  energy.Initialize(name + ".energy", 4096, 0, 4095, "a.u.");

  // timestamp : 
  timestamp.Initialize(name + ".timestamp", 48, 0, pow(2., 48)-1, 
                       "ns",true);
}

////
//
//
MyParameters::MyParameters(string name) 
{
  // create the 48 channels 
  for (size_t i=0; i<48; ++i) {
    chan[i].Initialize(name + to_string(i));
  }

  // initialize the multiplicity
  //  - 32 bins between -0.5 to 31.5.
  multiplicity.Initialize(name + ".mult", 32, -0.5, 30.5, "a.u.");
}




