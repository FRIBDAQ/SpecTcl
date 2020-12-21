#include "MyParameterMapper.h"
#include "MyParameters.h"
#include<mutex>

std::mutex mu;

// Make it possible to write DDASHit instead of DAQ::DDAS::DDASHit.
using DAQ::DDAS::DDASFitHit;

int max = -1;

////
//
MyParameterMapper::MyParameterMapper(MyParameters& params) :
  m_params(params),
  m_chanMap()
{
  m_chanMap[0] = 0;
  m_chanMap[1] = 144; 
  m_chanMap[2] = 272; 
}

MyParameterMapper::MyParameterMapper(const MyParameterMapper& rhs) :
  m_params(rhs.m_params),
  m_chanMap(rhs.m_chanMap)
{}

void MyParameterMapper::mapToParameters(const std::vector<DDASFitHit>& channelData,
                                        CEvent& rEvent)
{
  m_params.data.m_chanHit.clear();
  size_t nHits = channelData.size();

  // assign number of hits as event multiplicity
  m_params.multiplicity = nHits;

  // loop over all hits in event
  for (size_t i=0; i<nHits; ++i) {

    // convenience variable declared to refer to the i^th hit
    auto& hit = channelData[i];

    // Use the crate, slot, and channel to figure out the global
    // channel index.
    int globalChanIdx = computeGlobalIndex(hit);

    // Assign values to appropriate channel
    m_params.chan[globalChanIdx].energy    = hit.GetEnergy();
    m_params.chan[globalChanIdx].timestamp = hit.GetTime();

    m_params.data.m_chanHit.push_back(globalChanIdx);
  }
}


int MyParameterMapper::computeGlobalIndex(const DDASFitHit& hit) 
{
  int crateId = hit.GetCrateID();
  int slotIdx = hit.GetSlotID()-2;
  int chanIdx = hit.GetChannelID();

  const int nChanPerSlot = 16;

  return m_chanMap[crateId] + slotIdx*nChanPerSlot + chanIdx;
}
