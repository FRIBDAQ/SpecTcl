#include "MyParameters.h"

//________________________________________________________________________
// Initialize MyParameters.
//
MyParameters::MyParameters(std::string name) {
  multiplicity.Initialize(name + ".multiplicity", 32, 0, 31, "a.u.");
  energy.Initialize(name + ".energy", 32768, 0, 65535, "a.u.", 16, 0);
  timestamp.Initialize(name + ".timestamp", 32768, 0, std::pow(2, 64) - 1,
                       "a.u.", 16, 0);
}
