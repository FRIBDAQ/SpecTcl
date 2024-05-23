#include "MyParameterMapper.h"

#include "MyParameters.h"

// Make it possible to write DDASHit instead of DAQ::DDAS::DDASHit:
using DAQ::DDAS::DDASHit;

//________________________________________________________________________
// Initialize the parameters and channel map.
//
// To add additional crates:
//     m_chanMap[crateID] = startIndex;
//
// A full crate of Pixie-16 digitizers contains 208 channels. For a second
// crate, starting at channel 208, one would add the fllowing line:
//     m_chanMap[1] = 208;
//
MyParameterMapper::MyParameterMapper(MyParameters& params) :
    m_params(params), m_chanMap()
{
    // Crate 0 has one module which starts at global channel index 0:
    m_chanMap[0] = 0;
}

//________________________________________________________________________
// 
void MyParameterMapper::mapToParameters(const std::vector<DDASHit>& channelData,
                                        CEvent& rEvent)
{
    size_t nHits = channelData.size();

    // Assign number of hits as event multiplicity:
    m_params.multiplicity = nHits;

    // Loop over all hits in event:
    for (size_t i = 0; i < nHits; i++) {
	// Convenience variable declared to refer to the i^th hit
	auto& hit = channelData[i];
    
	// Use the crate, slot, and channel to figure out the global index:
	int globalChanIdx = computeGlobalIndex(hit);
    
	// Assign values to appropriate channel:
	m_params.chan[globalChanIdx].energy    = hit.GetEnergy();
	m_params.chan[globalChanIdx].timestamp = hit.GetTime();
    }
  
}

//________________________________________________________________________
// 
int MyParameterMapper::computeGlobalIndex(const DDASHit& hit) 
{
    int crateId = hit.GetCrateID();
    int slotIdx = hit.GetSlotID() - 2; // First module is in slot 2.
    int chanIdx = hit.GetChannelID();

    const int nChanPerSlot = 16;

    return m_chanMap[crateId] + slotIdx*nChanPerSlot + chanIdx;
}
