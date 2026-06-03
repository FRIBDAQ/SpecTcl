#ifndef MYPARAMETERS_H
#define MYPARAMETERS_H

#include <string>

#include <TreeParameter.h>
#include <config.h>

/**
 * @struct MyParameters
 * @brief This is the root of the tree structure. The name of this will be used
 * to name branches of the tree. The size of the parameter arrays is set in the
 * constructor. We assume for this example a system consisting of a single
 * 16-channel DDAS module, see MyParameters.cpp for details.
 */
struct MyParameters {

  CTreeParameter multiplicity;
  CTreeParameterArray energy;
  CTreeParameterArray timestamp;

  /**
   * @brief Constructor.
   * @param name Name of root.
   */
  MyParameters(std::string name);
};

#endif
