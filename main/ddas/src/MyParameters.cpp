#include "MyParameters.h"

/**
 * @details
 * We assume for this example that there is one 16-channel DDAS module in the
 * system. The multiplicity parameter is initialized with 32 bins between 0
 * and 31. The energy and timestamp arrays are initialized with 16 channels,
 * energy between 0 and 65535, and timestamp between 0 and 2^64-1.
 */
MyParameters::MyParameters(std::string name) {
  s_multiplicity.Initialize(name + ".mult", 32, 0, 31, "a.u.");
  s_energy.Initialize(name + ".energy", 32768, 0, 65535, "a.u.", 16, 0);
  s_timestamp.Initialize(name + ".timestamp", 64, 0, std::pow(2, 64) - 1, "ns",
                         true, 16, 0);
}
