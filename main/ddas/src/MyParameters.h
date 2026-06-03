#ifndef MYPARAMETERS_H
#define MYPARAMETERS_H

#include <string>

#include <TreeParameter.h>
#include <config.h>

/**
 * @struct MyParameters
 * @brief This is the root of the tree structure. The name of this will be used
 * to name branches of the tree. The size of the parameter arrays is set in the
 * constructor.
 */
struct MyParameters {

  CTreeParameter s_multiplicity;   //!< Multiplicity (number of hits).
  CTreeParameterArray s_energy;    //!< Raw energies for each channel.
  CTreeParameterArray s_timestamp; //!< Nanosecond timestamps for each channel.

  /**
   * @brief Constructor.
   * @param name Name of root.
   */
  MyParameters(std::string name);
};

#endif
