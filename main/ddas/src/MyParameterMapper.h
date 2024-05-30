#ifndef MYPARAMETERMAPPER_H
#define MYPARAMETERMAPPER_H

#include <ParameterMapper.h>

#include <map>

class CEvent;       // SpecTcl event, we don't use it.
class MyParameters; // The strucutre of tree parameters.

//________________________________________________________________________
// Class responsible for mapping DDAS hit data to SpecTcl TreeParameters
//
// It operates on a MyParameters structure containing TreeParameters but 
// does not own it.
//
class MyParameterMapper : public DAQ::DDAS::CParameterMapper
{
private:
    MyParameters&      m_params;  // Reference to the tree parameter structure.
    std::map<int, int> m_chanMap; // Global channel index for crates.

public:
    /**
     * @brief Constructor.
     *
     * @param params The data structure.
     */    
    MyParameterMapper(MyParameters& params);
    
    /**
     * @brief Map raw hit data to tree parameters.
     *
     * @param channelData The hit data.
     * @param rEvent      The SpecTcl event.
     */
    virtual void mapToParameters(
	const std::vector<DAQ::DDAS::DDASHit>& channelData, CEvent& rEvent
	);
    
    /**
     * @brief Compute channel index from crate, slot, and channel information.
     * 
     * @param hit The hit data.
     * 
     * @return The global channel index.
     */ 
    int computeGlobalIndex(const DAQ::DDAS::DDASHit& hit);
};

#endif

