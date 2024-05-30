#ifndef MYPARAMETERS_H
#define MYPARAMETERS_H

#include <string>

#include <config.h>
#include <TreeParameter.h>

//________________________________________________________________________
// The tree-like structure of data will be :
// 
//  MyParameters
//  |
//  +-- multiplicity
//  |
//  +-- ChannelData (chan[0])
//  |   +-- energy
//  |   \-- timestamp
//  |
//  +-- ChannelData (chan[1])
//  |   +-- energy
//  |   \-- timestamp
//  |
//  ... (13 more channel data objects)
//  |
//  \-- ChannelData (chan[15])
//      +-- energy
//      \-- timestamp
//
struct ChannelData {
    
    CTreeParameter energy;
    CTreeParameter timestamp;

    /**
     * @brief Initialize the TreeParameters
     * 
     * @details
     * We will create TreeParameters with names associated with the name 
     * passed in. For example, if name = "rawdata", then we will create 
     * TreeParameters with names rawdata.energy and rawdata.timestamp.
     *
     * @param name Name of parent in tree structure.
     */
    void Initialize(std::string name);
};

//________________________________________________________________________
// Struct for top-level events
// 
//  This contains 16 channels of data because we have 1 16-channel module
//  in our system. We also want to keep some information for the number of
//  DDAS hits per event (i.e. the multiplicity).
//
struct MyParameters {
    
    ChannelData    chan[16];
    CTreeParameter multiplicity;

    /**
     * @brief Constructor.
     * 
     * @details
     * This is the root of the tree structure. The name of this will be used 
     * to name branches of the tree.
     *
     * @param name Name of root.
     */
    MyParameters(std::string name);
};

#endif
