

#ifndef MYPARAMETERMAPPER_H
#define MYPARAMETERMAPPER_H

#include <ParameterMapper.h>

#include <map>

// SpecTcl event, we don't use it 
class CEvent; 

// The strucutre of tree parameters
class MyParameters;


// Class responsible for mapping DDAS hit data to SpecTcl TreeParameters
//
// It operates on a MyParameters structure containing TreeParameters but 
// does not own it.
class MyParameterMapper : public DAQ::DDAS::CParameterMapper
{
  private:
    MyParameters& m_params;           // reference to the tree parameter structure
    std::map<int, int> m_chanMap;     // global channel index for crates

  public:
    // Constructor.
    //
    //  \param params   the data structure
    MyParameterMapper(MyParameters& params);

    // Map raw hit data to TreeParameters
    //
    // \param channelData   the hit data
    // \param rEvent        the SpecTcl event
    virtual void mapToParameters(const std::vector<DAQ::DDAS::DDASHit>& channelData, 
                                  CEvent& rEvent);

    // Compute channel index from crate, slot, and channel information
    //
    // \param hit   the ddas hit
    int computeGlobalIndex(const DAQ::DDAS::DDASHit& hit);
};
#endif

