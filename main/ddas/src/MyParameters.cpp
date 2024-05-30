#include "MyParameters.h"

//________________________________________________________________________
// Initialize the ChannelData
//
void ChannelData::Initialize(std::string name) {
    // Energy: 32768 channels between 0 and 32767
    energy.Initialize(name + ".energy", 32768, 0, 32767, "a.u.");

    // Timestamp: 64 bits with 2^64-1 ns per bin starting at 0
    timestamp.Initialize(
	name + ".timestamp", 64, 0, std::pow(2,64)-1, "ns", true
	);
}

//________________________________________________________________________
// Initialize MyParameters. Calls ChannelData::Initialize() to initialize
// ChannelData for each channel in the system.
//
MyParameters::MyParameters(std::string name) 
{
    // Create the 16 channels 
    for (size_t i = 0; i < 16; i++) {
	chan[i].Initialize(name + std::to_string(i));
    }

    // Initialize the multiplicity: 32 bins between 0 and 31
    multiplicity.Initialize(name + ".mult", 32, 0, 31, "a.u.");
}
