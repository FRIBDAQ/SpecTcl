#ifndef MYPARAMETERS_H
#define MYPARAMETERS_H

#include <config.h>
#include <TreeParameter.h>
#include <string>
#include <PipelineData.h>
#include <vector>
#include "MyPipelineData.h"
#include "MyParameters2.h"

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

  ChannelData();
  ChannelData(const ChannelData& rhs);
  void Initialize(std::string name);
  void Reset();
};

//____________________________________________________________
// Struct for top-level events
// 
struct MyParameters {

  ChannelData      chan[1000];
  CTreeParameter   multiplicity;
  MyPipelineData   data;
  MyParameters2    example;
  
  // Ctor
  MyParameters(std::string name);
  // Dtor
  ~MyParameters(){};
  // Copy Ctor
  MyParameters(const MyParameters& rhs);

  void Reset();
};

#endif
