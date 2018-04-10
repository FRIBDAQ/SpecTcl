
#ifndef MYPARAMETERS_H
#define MYPARAMETERS_H

#include <config.h>
#include <TreeParameter.h>
#include <string>


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
//  ... (45 more channel data objects)
//  |
//  \-- ChannelData (chan[47])
//      +-- energy
//      \-- timestamp
//
//
struct ChannelData {

  CTreeParameter energy;
  CTreeParameter timestamp;

  // Initialize the TreeParameters
  //
  // We will create TreeParameters with names associated with
  // the name passed in. For example, if name = "rawdata", then 
  // we will create TreeParameters with names rawdata.energy and
  // rawdata.timestamp.
  //
  // \param name  name of parent in tree structure
  void Initialize(std::string name);
};

//____________________________________________________________
// Struct for top-level events
// 
//  This contains 48 channels of data because we have 3 modules
//  in our system, each with 16 channels a piece.
//
//  We also want to keep some information for the number of ddas hits
//  per event (i.e. the multiplicity)
struct MyParameters {

  ChannelData          chan[48];
  CTreeParameter   multiplicity;

  // Constructor
  //
  // This is the root of the tree structure. The name of this
  // will be used to name the branches of the tree.
  //
  // \param name  name of root
  MyParameters(std::string name);
};

#endif
