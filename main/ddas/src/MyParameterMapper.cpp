
#include "MyParameterMapper.h"
#include "MyParameters.h"

// Make it possible to write DDASHit instead of DAQ::DDAS::DDASHit.
using DAQ::DDAS::DDASHit;

////
//
MyParameterMapper::MyParameterMapper(MyParameters& params) 
  : m_params(params),
   m_chanMap()
{
  // crate 0 has 2 modules and thus accounts for 32 channels and
  // crate 1 doesnt exist. This means that the first channel index
  // of crate 2 is 32.
  m_chanMap[0] = 0;
  m_chanMap[2] = 32;
}

void MyParameterMapper::mapToParameters(const std::vector<DDASHit>& channelData,
                                        CEvent& rEvent)
{
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

  }

}

int MyParameterMapper::computeGlobalIndex(const DDASHit& hit) 
{
  int crateId = hit.GetCrateID();
  int slotIdx = hit.GetSlotID()-2;
  int chanIdx = hit.GetChannelID();

  const int nChanPerSlot = 16;

  return m_chanMap[crateId] + slotIdx*nChanPerSlot + chanIdx;
}
