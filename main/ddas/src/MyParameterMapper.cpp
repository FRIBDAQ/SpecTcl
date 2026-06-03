#include "MyParameterMapper.h"

#include "MyParameters.h"
#include <algorithm>

// Make it possible to write DDASHit instead of ddasfmt::DDASHit:
using ddasfmt::DDASHit;

/**
 * @details
 * Information about the DDAS modules in the system is used to build a map of
 * crate and slot IDs to a global channel index. This allows the mapToParameters
 * function to fill the appropriate TreeParameters based on the crate, slot, and
 * channel information in the raw hit data.
 * @note The channel map is contiguous (even if there are gaps in the physical
 * crate layout) and starts at 0. For example, if there is one 16-channel module
 * in slot 2 of crate 0, then the channels in that module will be mapped to
 * global channel indices 0-15. The next module would be mapped starting at
 * global channel index 16, and so on. The channel map orders the modules first
 * by crate ID and then by slot ID regardless of the order in which they are
 * provided in the `modules` vector.
 */
MyParameterMapper::MyParameterMapper(MyParameters &params) : m_params(params) {
  std::vector<ModuleInfo> modules = {
      {0, 2, 16}, // Crate 0, slot 2, 16 channels
                  // Add more modules here if needed
  };
  buildChannelMap(modules);
}

void MyParameterMapper::mapToParameters(const std::vector<DDASHit> &channelData,
                                        CEvent &rEvent) {
  size_t nHits = channelData.size();

  // Assign number of hits as event multiplicity:
  m_params.s_multiplicity = nHits;

  // Loop over all hits in event:
  for (size_t i = 0; i < nHits; i++) {
    // Convenience variable declared to refer to the i^th hit:
    auto &hit = channelData[i];

    // Use the crate, slot, and channel to figure out the global index:
    int idx = computeGlobalIndex(hit);

    // Assign values to appropriate channel:
    m_params.s_energy[idx] = hit.getEnergy();
    m_params.s_timestamp[idx] = hit.getTime();
  }
}

//________________________________________________________________________
// Private utilities:
//

/**
 * @details
 * We sort the `modules` vector in place to ensure a consistent ordering for
 * building the channel map based on crate and slot IDs. The map is built by
 * iterating over the sorted modules and assigning a contiguous block of global
 * channel indices to each module based on its number of channels. The base
 * index is incremented by the number of channels in each module to ensure that
 * the next module's channels are mapped to the correct starting index.
 * @note The `modules` vector is passed by reference and modified in place. If
 * you don't want this behavior, you can sort a copy of the vector instead.
 */
void MyParameterMapper::buildChannelMap(std::vector<ModuleInfo> &modules) {
  std::sort(modules.begin(), modules.end());
  int base = 0;
  for (const auto &mod : modules) {
    m_chanMap[{mod.s_crateId, mod.s_slotId}] = base;
    base += mod.s_nChannels;
  }
}

int MyParameterMapper::computeGlobalIndex(const DDASHit &hit) {
  int crateId = hit.getCrateID();
  int slotIdx = hit.getSlotID();
  int chanIdx = hit.getChannelID();

  auto key = std::make_pair(crateId, slotIdx);
  return m_chanMap.at(key) + chanIdx;
}
