#include "MyParameters.h"

using namespace std;

////
//
// 

MyPipelineData::MyPipelineData()
{
  m_chanHit.clear();
}

MyPipelineData::MyPipelineData(const MyPipelineData& rhs) :
  m_chanHit(rhs.m_chanHit)
{}

////
//
// 

void ChannelData::Initialize(string name) {
  // energy : 4096 channels between 0 and 4096
  energy.Initialize(name + ".energy", 65536, 0, 65535, "a.u.");

  // timestamp : 
  timestamp.Initialize(name + ".timestamp", 48, 0, pow(2., 48)-1, 
                       "ns",true);
}

void ChannelData::Reset()
{
  energy.Reset();
  timestamp.Reset();
}

////
//
//
MyParameters::MyParameters(string name) 
{
  // create the 400 channels 
  for (size_t i=0; i<400; ++i) {
    //chan[i].Initialize(name + to_string(i));
    Char_t detname[11];
    sprintf(detname,".raw.chan%03d",i);
    chan[i].Initialize(name+detname);
  }

  // initialize the multiplicity
  //  - 32 bins between -0.5 to 31.5.
  multiplicity.Initialize(name+".multiplicity", 32, -0.5, 30.5, "a.u.");
}

MyParameters::MyParameters(const MyParameters& rhs):
  chan(rhs.chan),
  data(rhs.data),
  multiplicity(rhs.multiplicity)
{
  CTreeParameter::BindParameters();
}

void
MyParameters::Reset()
{
  data.m_chanHit.clear();
  for (size_t i=0; i<400; ++i) {
    chan[i].Reset();
  }
  multiplicity = 0;  

}


