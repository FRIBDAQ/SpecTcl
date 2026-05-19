#ifndef MYPARAMETERS_H
#define MYPARAMETERS_H

#include <string>

#include <config.h>
#include <TreeParameter.h>

//________________________________________________________________________
// Struct for top-level events
// 
//  This contains 16 channels of data because we have 1 16-channel module
//  in our system. We also want to keep some information for the number of
//  DDAS hits per event (i.e. the multiplicity).
//
struct MyParameters {

    CTreeParameter multiplicity;
    CTreeParameterArray energy;
    CTreeParameterArray timestamp;

    /**
     * @brief Constructor.
     * @details
     * This is the root of the tree structure. The name of this will be used 
     * to name branches of the tree.
     * @param name Name of root.
     */
    MyParameters(std::string name);
};

#endif
